#include <Simulator/Structs/Simulation.h>
#include <Simulator/LIFCompartmental/LIFCNeuron.h>

#include <algorithm>
#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {

LIFCNeuron::LIFCNeuron(const CoreStructs::LIFCNeuronStruct & lifcneuronstruct, Simulation & _Sim): BSNeuron(scneuronstruct.ID), Sim(_Sim) {
    build_data = lifcneuronstruct;

    Class_ = CoreStructs::_LIFCNeuron;

    Vm_mV = lifcneuronstruct.RestingPotential_mV; // Initialized at rest
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
    g_peak_ADP_max_nS = g_peak_ADP_nS*ADP_saturation_multiplier;
    tau_recovery_ADP_ms = lifcneuronstruct.AfterDepolarizationRecoveryTime_ms;
    ADP_depletion = lifcneuronstruct.AfterDepolarizationDepletion;

    // Use AfterDepolarizationSaturationModel from buid_data directly

    dh_spike = lifcneuronstruct.AdaptiveThresholdDiffPerSpike;
    tau_h_ms = lifcneuronstruct.AdaptiveTresholdRecoveryTime_ms;
    dVth_mV = lifcneuronstruct.AdaptiveThresholdDiffPotential_mV;
    Vth_floor_mV = lifcneuronstruct.AdaptiveThresholdFloor_mV;
    delta_floor_per_spike_mV = lifcneuronstruct.AdaptiveThresholdFloorDeltaPerSpike_mV;
    tau_floor_decay_ms = lifcneuronstruct.AdaptiveThresholdFloorRecoveryTime_ms;

}

//! Returns the geometric center of the neuron.
//! In case the soma is constructed of multiple compartments,
//! this is a center of gravity of the set of compartment centers.
Geometries::Vec3D &LIFCNeuron::GetCellCenter() {
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

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
