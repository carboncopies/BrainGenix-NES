#include <Simulator/Structs/Simulation.h>
#include <Simulator/LIFCompartmental/LIFCNeuron.h>
#include <Simulator/Structs/Receptor.h>

#include <algorithm>
#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {

// Note that compute_normalization() and g_norm() are in Receptor.h.

LIFCNeuron::LIFCNeuron(const CoreStructs::LIFCNeuronStruct & lifcneuronstruct, Simulation & _Sim): BSNeuron(lifcneuronstruct.ID), Sim(_Sim) {
    build_data = lifcneuronstruct;

    Class_ = CoreStructs::_LIFCNeuron;

    VRest_mV = lifcneuronstruct.RestingPotential_mV;
    VReset_mV = lifcneuronstruct.ResetPotential_mV;
    VAct_mV = lifcneuronstruct.SpikeThreshold_mV;
    Rm_GOhm = lifcneuronstruct.MembraneResistance_MOhm/1000.0; // Note the unit conversion needed!
    Cm_pF = lifcneuronstruct.MembraneCapacitance_pF;
    tau_absref_ms = lifcneuronstruct.RefractoryPeriod_ms;
    VSpike_mV = lifcneuronstruct.SpikeDepolarization_mV;

    // Use UpdateMethod and ResetMethod from build_data directly

    E_AHP_mV = lifcneuronstruct.AfterHyperpolarizationReversalPotential_mV;
    tau_rise_fAHP_ms = lifcneuronstruct.FastAfterHyperpolarizationRise_ms;
    tau_decay_fAHP_ms = lifcneuronstruct.FastAfterHyperpolarizationDecay_ms;
    g_peak_fAHP_nS = lifcneuronstruct.FastAfterHyperpolarizationPeakConductance_nS;
    g_peak_fAHP_max_nS = lifcneuronstruct.FastAfterHyperpolarizationMaxPeakConductance_nS;
    Kd_fAHP_nS = lifcneuronstruct.FastAfterHyperpolarizationHalfActConstant;

    tau_rise_sAHP_ms = lifcneuronstruct.SlowAfterHyperpolarizationRise_ms;
    tau_decay_sAHP_ms = lifcneuronstruct.SlowAfterHyperpolarizationDecay_ms;
    g_peak_sAHP_nS = lifcneuronstruct.SlowAfterHyperpolarizationPeakConductance_nS;
    g_peak_sAHP_max_nS = lifcneuronstruct.SlowAfterHyperpolarizationMaxPeakConductance_nS;
    Kd_sAHP_nS = lifcneuronstruct.SlowAfterHyperpolarizationHalfActConstant;

    // Use AfterHyperpolarizationSaturationModel from buid_data directly

    fatigue_threshold = lifcneuronstruct.FatigueThreshold;
    tau_fatigue_recovery_ms = lifcneuronstruct.FatigueRecoveryTime_ms;

    E_ADP_mV = lifcneuronstruct.AfterDepolarizationReversalPotential_mV;
    tau_rise_ADP_ms = lifcneuronstruct.AfterDepolarizationRise_ms;
    tau_decay_ADP_ms = lifcneuronstruct.AfterDepolarizationDecay_ms;
    g_peak_ADP_nS = lifcneuronstruct.AfterDepolarizationPeakConductance_nS;
    ADP_saturation_multiplier = lifcneuronstruct.AfterDepolarizationSaturationMultiplier;
    tau_recovery_ADP_ms = lifcneuronstruct.AfterDepolarizationRecoveryTime_ms;
    ADP_depletion = lifcneuronstruct.AfterDepolarizationDepletion;

    // Use AfterDepolarizationSaturationModel from buid_data directly

    dh_spike = lifcneuronstruct.AdaptiveThresholdDiffPerSpike;
    tau_h_ms = lifcneuronstruct.AdaptiveTresholdRecoveryTime_ms;
    dVth_mV = lifcneuronstruct.AdaptiveThresholdDiffPotential_mV;
    Vth_floor_mV = lifcneuronstruct.AdaptiveThresholdFloor_mV;
    delta_floor_per_spike_mV = lifcneuronstruct.AdaptiveThresholdFloorDeltaPerSpike_mV;
    tau_floor_decay_ms = lifcneuronstruct.AdaptiveThresholdFloorRecoveryTime_ms;

    // Additional variables initialization
    Vm_mV = VRest_mV;
    tau_m_ms = Rm_GOhm * Cm_pF;
    g_L_nS = 1.0 / Rm_GOhm;
    g_peak_ADP_max_nS = g_peak_ADP_nS*ADP_saturation_multiplier;

    norm_fAHP = Connections::compute_normalization(tau_rise_fAHP_ms, tau_decay_fAHP_ms);
    norm_sAHP = Connections::compute_normalization(tau_rise_sAHP_ms, tau_decay_sAHP_ms);
    norm_ADP = Connections::compute_normalization(tau_rise_ADP_ms, tau_decay_ADP_ms);
}

//! Returns the geometric center of the neuron.
//! In case the soma is constructed of multiple compartments,
//! this is a center of gravity of the set of compartment centers.
Geometries::Vec3D& LIFCNeuron::GetCellCenter() {
    Geometries::Vec3D geoCenter_um{};
    for (const auto & CompID : build_data.SomaCompartmentIDs) {
        if (CompID < LIFCCompartments.size()) {
            int ShapeID = LIFCCompartments.at(CompID).ShapeID;
            auto ShapePtr = Sim.FindShapeByID(ShapeID);
            if (ShapePtr != nullptr) {
                geoCenter_um = geoCenter_um + ShapePtr->Center_um;
            }
        }
    }
    GeoCenter_um = geoCenter_um / float(build_data.SomaCompartmentIDs.size());
    return GeoCenter_um;
};

void LIFCNeuron::UpdateType(Connections::NeurotransmitterType neurotransmitter) {
    // Only if still unknown.
    if (Type_==CoreStructs::UnknownNeuron) {
        auto it = NeurotransmitterType2NeuronType.find(neurotransmitter);
        if (it != NeurotransmitterType2NeuronType.end()) {
            Type_ = it->second;
        }
    }
}

void LIFCNeuron::spike(float tfire) {
    // Spike logging
    updates_since_spike = 0;
    t_last_spike = tfire;
    TAct_ms.push_back(t_last_spike);
    
    // Membrane potential reset
    if (build_data.UpdateMethod == CoreStructs::CLASSICAL) {
        Vm_mV = VReset_mV;
    } else {
        if (build_data.ResetMethod == CoreStructs::TOVM) {
            VReset_mV = Vm_mV; // Remember value before AP
        }
        Vm_mV = VSpike_mV;
    }
    reset_done = false;
    
    // Threshold effects
    // a. nonlinear hard-cap
    if (fatigue_threshold > 0) {
        fatigue += 1.0;
    }
    // b. Adaptive threshold models sodium channel inactivation
    h_spike -= dh_spike;
    // c. Dynamic threshold floor
    if (delta_floor_per_spike_mV > 0) {
        Vth_floor_mV += delta_floor_per_spike_mV;
    }
    
    // ADP saturation
    if (build_data.AfterDepolarizationSaturationModel != CoreStructs::ADPCLIP) {
        a_ADP -= ADP_depletion;
    }
    
    // STDP
    if (Sim.STDP) {
        for (auto& RDataptr : LIFCReceptorDataVec) {
            RDataptr->STDP_Update(tfire);
        }
    }
}

void LIFCNeuron::check_spiking(float t, float V_th_adaptive) {
    if (next_directstim_idx < TDirectStim_ms.size()) {
        float tFire_ms = TDirectStim_ms.at(next_directstim_idx);
        if (tFire_ms <= t) {
            TAct_ms.push_back(tFire_ms);
            spike(tFire_ms);
            next_directstim_idx++;
            return;
        }
    }
    
    if ((fatigue_threshold > 0) && (fatigue > fatigue_threshold)) {
        return;
    }

    if (TauSpont_ms.stdev != 0) {
        if (t >= TSpontNext_ms) {
            spike(t);

            // Obtain interval to the next spontaneous event from the distribution.
            float dt_spont = DtSpontDist->RandomSample(1)[0]; // Generate 1 random sample in a vector, take the 0th element.
            TSpontNext_ms = t + dt_spont;
            return;
        }
    }
    
    if (Vm_mV >= V_th_adaptive) {
        spike(t);
    }
}

void LIFCNeuron::update_conductances(float t) {
    // Update PSP conductances
    for (auto& RDataptr : LIFCReceptorDataVec) {
        RDataptr->Update_Conductance(t, Vm_mV);
    }
    
    // Update fAHP, sAHP, ADP conductances
    float g_fAHP_linear = g_peak_fAHP_nS * Connections::g_norm(t, TAct_ms, tau_rise_fAHP_ms, tau_decay_fAHP_ms, norm_fAHP, 0);
    float g_sAHP_linear = g_peak_sAHP_nS * Connections::g_norm(t, TAct_ms, tau_rise_sAHP_ms, tau_decay_sAHP_ms, norm_sAHP, 0);
    if (build_data.AfterHyperpolarizationSaturationModel == CoreStructs::AHPCLIP) {
        g_fAHP_nS = std::min(g_fAHP_linear, g_peak_fAHP_max_nS);
        g_sAHP_nS = std::min(g_sAHP_linear, g_peak_sAHP_max_nS);
    } else {
        g_fAHP_nS = g_peak_fAHP_max_nS * (g_fAHP_linear / (g_fAHP_linear + Kd_fAHP_nS));
        g_sAHP_nS = g_peak_sAHP_max_nS * (g_sAHP_linear / (g_sAHP_linear + Kd_sAHP_nS));
    }
    
    float g_ADP_linear = g_peak_ADP_nS * Connections::g_norm(t, TAct_ms, tau_rise_ADP_ms, tau_decay_ADP_ms, norm_ADP, 0);
    if (build_data.AfterDepolarizationSaturationModel == CoreStructs::ADPCLIP) {
        g_ADP_nS = std::min(g_ADP_linear, g_peak_ADP_max_nS);
    } else {
        a_ADP = a_ADP + (1 - a_ADP) * tDiff_ms / tau_recovery_ADP_ms;
        a_ADP = std::max(0.0f, std::min(1.0f, a_ADP));
        g_ADP_nS = a_ADP * g_ADP_linear;
    }
}

float LIFCNeuron::update_currents() {
    float I = g_fAHP_nS * (Vm_mV - E_AHP_mV) +
              g_sAHP_nS * (Vm_mV - E_AHP_mV) +
              g_ADP_nS * (Vm_mV - E_ADP_mV);
    
    for (auto& RDataptr : LIFCReceptorDataVec) {
        I += RDataptr->Get_Current(Vm_mV); // p->g * (Vm_mV - p->E)
    }
    
    return I;
}

float LIFCNeuron::update_membrane_potential_forward_Euler(float I) {
    float dV = (-(Vm_mV - VRest_mV) + Rm_GOhm * (-I)) * tDiff_ms / tau_m_ms;
    Vm_mV += dV;
    return dV;
}

void LIFCNeuron::update_membrane_potential_exponential_Euler_Rm() {
    float tau_eff = tau_m_ms / (1 + Rm_GOhm * (g_fAHP_nS + g_sAHP_nS + g_ADP_nS));
    float sum_g = 0.0;
    float sum_gE = 0.0;
    
    for (auto& RDataptr : LIFCReceptorDataVec) {
        sum_g += RDataptr->g();
        sum_gE += RDataptr->gE_k(); // p->g * p->E
    }
    
    float V_inf = (g_fAHP_nS * E_AHP_mV + g_sAHP_nS * E_AHP_mV + g_ADP_nS * E_ADP_mV + sum_gE + (1 / Rm_GOhm) * VRest_mV) /
                  (g_fAHP_nS + g_sAHP_nS + g_ADP_nS + sum_g + (1 / Rm_GOhm));
    
    Vm_mV = V_inf + (Vm_mV - V_inf) * exp(-tDiff_ms / tau_eff);
}

void LIFCNeuron::update_membrane_potential_exponential_Euler_Cm() {
    float sum_g = 0.0;
    float sum_gE = 0.0;
    
    for (auto& RDataptr : LIFCReceptorDataVec) {
        sum_g += RDataptr->g();
        sum_gE += RDataptr->gE_k(); // p->g * p->E
    }
    
    float g_total = g_L_nS + g_fAHP_nS + g_sAHP_nS + g_ADP_nS + sum_g;
    float E_total = (g_L_nS * VRest_mV +
                     g_fAHP_nS * E_AHP_mV +
                     g_sAHP_nS * E_AHP_mV +
                     g_ADP_nS * E_ADP_mV +
                     sum_gE) / g_total;
    
    float tau_eff = Cm_pF / g_total;
    Vm_mV = E_total + (Vm_mV - E_total) * exp(-tDiff_ms / tau_eff);
}

float LIFCNeuron::update_adaptive_threshold() {
    h_spike += tDiff_ms * (1 - h_spike) / tau_h_ms;
    if (delta_floor_per_spike_mV > 0) {
        Vth_floor_mV -= tDiff_ms * (Vth_floor_mV - VAct_mV) / tau_floor_decay_ms;
    }
    float V_th_adaptive = std::max(
        VAct_mV + dVth_mV * (1 - h_spike),
        Vth_floor_mV
    );
    return V_th_adaptive;
}

void LIFCNeuron::update_with_classical_reset_clamp(float t) {
    float I = update_currents();
    
    if (t < (t_last_spike + tau_absref_ms)) {
        Vm_mV = VReset_mV;
        return;
    }
    
    float dV = update_membrane_potential_forward_Euler(I);
    
    float V_th_adaptive = update_adaptive_threshold();
    
    // Check possible spiking:
    check_spiking(t, V_th_adaptive);
}

void LIFCNeuron::update_with_reset_options(float t) {
    // (Option:) Spike onset drives membrane potential below threshold.
    if (build_data.ResetMethod == CoreStructs::ONSET) {
        if (updates_since_spike == 1) {
            Vm_mV = VReset_mV;
        }
    }
    
    float dV = 0;
    if (build_data.UpdateMethod == CoreStructs::EXPEULER_CM) {
        update_membrane_potential_exponential_Euler_Rm();
    } else if (build_data.UpdateMethod == CoreStructs::EXPEULER_RM) {
        update_membrane_potential_exponential_Euler_Cm();
    } else {
        float I = update_currents();
        dV = update_membrane_potential_forward_Euler(I);
    }
    
    float V_th_adaptive = update_adaptive_threshold();
    
    if (t >= (t_last_spike + tau_absref_ms)) {
        // (Option:) Drive membrane potential below threshold after absolute refractory period.
        if ((build_data.ResetMethod != CoreStructs::ONSET) && (!reset_done)) {
            Vm_mV = VReset_mV + dV;
            reset_done = true;
        }
        
        // Check possible spiking:
        check_spiking(t, V_th_adaptive);
    }
}

// Search for SrcNeuron-DstNeuron-ReceptorType triplet in LIFCReceptorDataVec.
CoreStructs::LIFCReceptorData* LIFCNeuron::FindLIFCReceptorPairing(LIFCNeuron* SrcNeuronPtr, Connections::LIFCReceptor* RPtr) {
    // We already know the DstNeuron ID is this neuron's ID, so we're looking for SrcNeuron match and receptor type match
    for (auto& LIFCRDataptr : LIFCReceptorDataVec) {
        if (LIFCRDataptr->SrcNeuronPtr == SrcNeuronPtr) {
            if (LIFCRDataptr->Type() == RPtr->Neurotransmitter) return LIFCRDataptr;
        }
    }
    return nullptr;
}

void LIFCNeuron::Calculate_Abstracted_PSP_Medians() {
    for (auto& RData : LIFCReceptorDataVec) {
        RData->Calculate_Abstracted_PSP_Medians();
    }
}

void LIFCNeuron::Update(float t_ms, bool recording) {
    assert(t_ms >= 0.0);

    tDiff_ms = t_ms - T_ms;
    if (tDiff_ms < 0) return;

    if (!abstracted_medians) { // Prepared once at the start of the simulation
        if (Sim.use_abstracted_LIF_receptors) Calculate_Abstracted_PSP_Medians();
        abstracted_medians = true;
    }

    // Hard-cap nonlinear spiking fatigue threshold.
    if (fatigue_threshold > 0) {
        fatigue -= tDiff_ms / tau_fatigue_recovery_ms;
        fatigue = std::max(fatigue, 0.0f);
    }
    
    update_conductances(t_ms);
    
    if (build_data.UpdateMethod == CoreStructs::CLASSICAL) {
        update_with_classical_reset_clamp(t_ms);
    } else {
        update_with_reset_options(t_ms);
    }

    // FIFO update if needed for something like Calcium imaging
    if (!FIFO.empty()) {
        FIFO.pop_front();
        float v = VRest_mV - Vm_mV;
        v = v < 0.0 ? 0.0 : v / 20.0; // *** Not clear to me what the sign and scaling should really be here.
        FIFO.push_back(v);
    }

    if (recording) {
        Record(t_ms);
    }

    T_ms = t_ms;
    updates_since_spike++; // *** Could add if (t_last_spike>=0.0) if it helps with overruns.
};

void LIFCNeuron::InputReceptorAdded(CoreStructs::LIFCReceptorData* RData) {
    LIFCReceptorDataVec.emplace_back(RData);

}

void LIFCNeuron::OutputTransmitterAdded(CoreStructs::LIFCReceptorData* RData) {
    LIFCTransmitterDataVec.emplace_back(RData);

}

const std::map<Connections::NeurotransmitterType, int> NeurotransmitterType2ConnectionType = {
    { Connections::AMPA, 1 },
    { Connections::GABA, 2 },
    { Connections::NMDA, 3 },
};

int GetConnectionType(Connections::NeurotransmitterType neurotransmitter) {
    auto it = NeurotransmitterType2ConnectionType.find(neurotransmitter);
    if (it == NeurotransmitterType2ConnectionType.end()) return 0; // Unknown type.
    return it->second;
}

void LIFCNeuron::GetConnectomeTargetsJSON(nlohmann::json& targetvec, nlohmann::json& typevec, nlohmann::json& weightvec) {
    for (auto & rdata : LIFCTransmitterDataVec) {
        targetvec.push_back(rdata->DstNeuronID);
        typevec.push_back(GetConnectionType(rdata->Type()));
        weightvec.push_back(rdata->weight * rdata->g_peak_sum_nS);
    }   
}

bool LIFCNeuron::UpdatePrePostStrength(int PresynapticID, float NewConductance_nS) {
    bool AMPA_found = false;
    for (auto& _ReceptorDataptr : LIFCReceptorDataVec) {
        if (_ReceptorDataptr->SrcNeuronID == PresynapticID) {
            if ((!AMPA_found) && (_ReceptorDataptr->Type()==Connections::AMPA)) {
                // The first one found is modified.
                _ReceptorDataptr->weight = NewConductance_nS/_ReceptorDataptr->g_peak_sum_nS;
                AMPA_found = true;
            } else {
                // Others are cleared to zero.
                _ReceptorDataptr->weight = 0.0;
            }
        }
    }

    return AMPA_found;
}

// Pure abstraction count of number of synapses.
// In order to not double-count, this counts AMPA and GABA but not NMDA.
size_t LIFCNeuron::GetAbstractConnection(int PreSynID, bool NonZero) {
    size_t NumReceptors = 0;
    for (auto& _ReceptorDataptr : LIFCReceptorDataVec) {
        if (_ReceptorDataptr->SrcNeuronID == PreSynID) {
            if (_ReceptorDataptr->Type() != Connections::NMDA) {
                if (NonZero) {
                    if (_ReceptorDataptr->weight != 0.0) {
                        NumReceptors += _ReceptorDataptr->ReceptorPtrs.size();
                    }
                } else {
                    NumReceptors += _ReceptorDataptr->ReceptorPtrs.size();
                }
            }

        }
    }

    return NumReceptors;
}

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
