#include <Simulator/Structs/Simulation.h>
#include <Simulator/LIFCompartmental/LIFCNeuron.h>

#include <algorithm>
#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {

// Helper function to compute normalization factor
float compute_normalization(float tau_rise, float tau_decay) {
    if (tau_rise == tau_decay) {
        throw std::invalid_argument("tau_rise must be different from tau_decay for normalization.");
    }
    float t_peak = (tau_rise * tau_decay) / (tau_decay - tau_rise) * log(tau_decay / tau_rise);
    float norm = exp(-t_peak / tau_decay) - exp(-t_peak / tau_rise);
    return norm;
}

// Function to compute normalized synaptic conductance
float g_norm(float t, const std::vector<float>& spike_times, float tau_rise, float tau_decay, 
             float norm, float onset_delay, float spike_dt_delta = 1000, float history_delta = 0.001) {
    t -= onset_delay;
    history_delta *= norm;
    double gnorm = 0;
    
    // Iterate in reverse to process most recent spikes first
    for (auto it = spike_times.rbegin(); it != spike_times.rend(); ++it) {
        double spike_dt = t - *it;
        if (spike_dt >= 0) {
            double g_norm_contribution = exp(-spike_dt / tau_decay) - exp(-spike_dt / tau_rise);
            if (spike_dt > spike_dt_delta && g_norm_contribution < history_delta) {
                return gnorm / norm;
            }
            gnorm += g_norm_contribution;
        }
    }
    return gnorm / norm;
}

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

    fatigue_treshold = lifcneuronstruct.FatigueThreshold;
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

    norm_fAHP = compute_normalization(tau_rise_fAHP, tau_decay_fAHP);
    norm_sAHP = compute_normalization(tau_rise_sAHP, tau_decay_sAHP);
    norm_ADP = compute_normalization(tau_rise_ADP, tau_decay_ADP);
}

//! Returns the geometric center of the neuron.
//! In case the soma is constructed of multiple compartments,
//! this is a center of gravity of the set of compartment centers.
Geometries::Vec3D& LIFCNeuron::GetCellCenter() {
    Geometries::Vec3D geoCenter_um{};
    for (const auto & CompID : build_data.SomaCompartmentIDs) {
        auto CompPtr = Sim.FindCompartmentByID(CompID);
        assert(CompPtr != nullptr);
        auto ShapeID = CompPtr->ShapeID;
        auto ShapePtr = Sim.FindShapeByID(ShapeID);
        assert(ShapePtr != nullptr);
        geoCenter_um = geoCenter_um + ShapePtr->Center_um;
    }
    GeoCenter_um = geoCenter_um / float(build_data.SomaCompartmentIDs.size());
    return GeoCenter_um;
};

void LIFCNeuron::spike(size_t i, float tfire) {
    // Spike logging
    last_spike_idx = static_cast<long>(i);
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
    if (fatigue_treshold > 0) {
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
    if (Sim->STDP) {
        for (auto p : presyn) {
            p->stdp_update(tfire);
        }
    }
}

void LIFCNeuron::check_spiking(size_t i, float t, float V_th_adaptive) {
    if (next_directstim_idx < TDirectStim_ms.size()) {
        float tFire_ms = TDirectStim_ms.at(next_directstim_idx);
        if (tFire_ms <= t) {
            TAct_ms.push_back(tFire_ms);
            spike(i, tFire_ms);
            next_directstim_idx++;
            return;
        }
    }
    
    if ((fatigue_treshold > 0) && (fatigue > fatigue_threshold)) {
        return;
    }
    
    if (Vm_mV >= V_th_adaptive) {
        spike(i, t);
    }
}

void LIFCNeuron::update_conductances(size_t i, float t) {
    // Update PSP conductances
    for (auto p : presyn) {
        p->update(i, t, Vm);
    }
    
    // Update fAHP, sAHP, ADP conductances
    float g_fAHP_linear = g_peak_fAHP_nS * g_norm(t, TAct_ms, tau_rise_fAHP_ms, tau_decay_fAHP_ms, norm_fAHP, 0);
    float g_sAHP_linear = g_peak_sAHP_nS * g_norm(t, TAct_ms, tau_rise_sAHP_ms, tau_decay_sAHP_ms, norm_sAHP, 0);
    if (build_data.AfterHyperpolarizationSaturationModel == CoreStructs::AHPCLIP) {
        g_fAHP_nS = std::min(g_fAHP_linear, g_peak_fAHP_max_nS);
        g_sAHP_nS = std::min(g_sAHP_linear, g_peak_sAHP_max_nS);
    } else {
        g_fAHP_nS = g_peak_fAHP_max_nS * (g_fAHP_linear / (g_fAHP_linear + Kd_fAHP_nS));
        g_sAHP_nS = g_peak_sAHP_max_nS * (g_sAHP_linear / (g_sAHP_linear + Kd_sAHP_nS));
    }
    
    float g_ADP_linear = g_peak_ADP_nS * g_norm(t, TAct_ms, tau_rise_ADP_ms, tau_decay_ADP_ms, norm_ADP, 0);
    if (build_data.AfterDepolarizationSaturationModel == CoreStructs::ADPCLIP) {
        g_ADP_nS = std::min(g_ADP_linear, g_peak_ADP_max_nS);
    } else {
        a_ADP = a_ADP + (1 - a_ADP) * tDiff_ms / tau_recovery_ADP_ms;
        a_ADP = std::max(0.0, std::min(1.0, a_ADP));
        g_ADP_nS = a_ADP * g_ADP_linear;
    }
}

float LIFCNeuron::update_currents() {
    float I = g_fAHP_nS * (Vm_mV - E_AHP_mV) +
              g_sAHP_nS * (Vm_mV - E_AHP_mV) +
              g_ADP_nS * (Vm_mV - E_ADP_mV);
    
    for (auto p : presyn) {
        I += p->g * (Vm_mV - p->E);
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
    
    for (auto p : presyn) {
        sum_g += p->g;
        sum_gE += p->g * p->E;
    }
    
    float V_inf = (g_fAHP_nS * E_AHP_mV + g_sAHP_nS * E_AHP_mV + g_ADP_nS * E_ADP_mV + sum_gE + (1 / Rm_GOhm) * VRest_mV) /
                  (g_fAHP_nS + g_sAHP_nS + g_ADP_nS + sum_g + (1 / Rm_GOhm));
    
    Vm_mV = V_inf + (Vm_mV - V_inf) * exp(-tDiff_ms / tau_eff);
}

void LIFCNeuron::update_membrane_potential_exponential_Euler_Cm() {
    float sum_g = 0.0;
    float sum_gE = 0.0;
    
    for (auto p : presyn) {
        sum_g += p->g;
        sum_gE += p->g * p->E;
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

void LIFCNeuron::update_with_classical_reset_clamp(size_t i, float t) {
    float I = update_currents();
    
    if (t < (t_last_spike + tau_absref_ms)) {
        Vm_mV = VReset_mV;
        return;
    }
    
    float dV = update_membrane_potential_forward_Euler(I);
    
    float V_th_adaptive = update_adaptive_threshold();
    
    // Check possible spiking:
    check_spiking(i, t, V_th_adaptive);
}

void LIFCNeuron::update_with_reset_options(size_t i, float t) {
    // (Option:) Spike onset drives membrane potential below threshold.
    if (build_data.ResetMethod == CoreStructs::ONSET) {
        if ((last_spike_idx + 1) == i) {
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
        check_spiking(i, t, V_th_adaptive);
    }
}

void LIFCNeuron::update(size_t i, float t) {
    tDiff_ms = t - T_ms;

    // Hard-cap nonlinear spiking fatigue threshold.
    if (fatigue_treshold > 0) {
        fatigue -= tDiff_ms / tau_fatigue_recovery_ms;
        fatigue = std::max(fatigue, 0.0);
    }
    
    update_conductances(i, t);
    
    if (build_data.UpdateMethod == CoreStructs::CLASSICAL) {
        update_with_classical_reset_clamp(i, t);
    } else {
        update_with_reset_options(i, t);
    }

    T_ms = t;
}

void LIFCNeuron::record(size_t i) {
    TRecorded_ms.push_back(T_ms);
    VmRecorded_mV.push_back(Vm_mV);
}


}; // namespace Simulator
}; // namespace NES
}; // namespace BG
