//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the functionally detailed LIF
                 Compartmental struct, representing a basic building block
                 for a functionally precise LIF Compartmental neural circuit.
    Additional Notes: None
    Date Created: 2025-07-29
*/

#pragma once

#include <nlohmann/json.hpp>

#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Structs/Receptor.h>

namespace BG {
namespace NES {
namespace Simulator {

// Forward declaration.
// This is used to give LIFCNeuron knowledge of the Simulation it is in, so
// That it can use compartment IDs and more to find its compartments and
// other related objects without needing cached pointers to them and without
// having to supply a whole list of references, since those references are
// all already available in the handy Simulation object.
struct Simulation;

const std::map<Connections::NeurotransmitterType, CoreStructs::NeuronType> NeurotransmitterType2NeuronType = {
    { Connections::AMPA, CoreStructs::GenericPrincipalNeuron },
    { Connections::GABA, CoreStructs::GenericInterneuron },
    { Connections::NMDA, CoreStructs::GenericPrincipalNeuron }
};

/**
 * @brief Provides object data and member functions of the LIF Compartmental
 *        neuron class.
 */
class LIFCNeuron: public BallAndStick::BSNeuron {
public:
    Simulation & Sim;
protected:
    Geometries::Vec3D GeoCenter_um{}; // This cache is updated whenever GetCellCenter() is called.

public:
    CoreStructs::LIFCNeuronStruct build_data; // Copy of the struct that was used to build this neuron.

    float VReset_mV = -55.0;
    float Rm_GOhm = 0.1; // 0.1-0.3 GΩ pyramidal
    float Cm_pF = 100.0; // 100-300 pF pyramidal
    float tau_absref_ms = 2.0;
    float VSpike_mV = 30.0;

    // Fast and slow after-hyperpolarization
    float E_AHP_mV = -90.0; // reversal potential (used for both slow and fast AHP)
    float tau_rise_fAHP_ms = 2.5; // 1-5 ms typical pyramidal
    float tau_decay_fAHP_ms = 30.0; // 20-50 ms typical pyramidal
    float g_peak_fAHP_nS = 3.0; // 1-5 nS
    float g_peak_fAHP_max_nS = 5.0; // 3-6 nS pyramidal
    float Kd_fAHP_nS = 1.5; // 1.5-3 nS half-activation constant for sigmoidal AHP saturation model

    float tau_rise_sAHP_ms = 30.0; // 20-50 ms typical pyramidal
    float tau_decay_sAHP_ms = 300.0; // 150-1000 ms typical pyramidal
    float g_peak_sAHP_nS = 1.0; // 0.5-3 nS
    float g_peak_sAHP_max_nS = 2.0; // 0.5-2.5 nS pyramidal
    float Kd_sAHP_nS = 0.3; // 0.3-1 nS half-activation constant for sigmoidal AHP saturation model

    // Hard-cap fatigue threshold
    float fatigue_threshold = 300; // 0 means not applied, number of spikes in a burst
    float tau_fatigue_recovery_ms = 1000.0;

    // After-depolarization (in select neuron types, otherwise g_peak_ADP_nS = 0)
    float E_ADP_mV = -20; // reversal potential
    float tau_rise_ADP_ms = 20.0;
    float tau_decay_ADP_ms = 200.0;
    float g_peak_ADP_nS = 0.0; // E.g. 0.3 pyramidal
    float ADP_saturation_multiplier = 2.0; // typically between 2-3 times
    float tau_recovery_ADP_ms = 300.0; // for resource availability mode, 200-600 ms for slow ADP recovery times
    float ADP_depletion = 0.3; // 0.2-0.4 per spike, ADP resource availability model

    // Adaptive threshold modeling sodium inactivation
    float dh_spike = 0.2;
    float tau_h_ms = 50.0; // 50 - 200 ms
    float dVth_mV = 10.0;
    float Vth_floor_mV = -50.0; // dynamic threshold floor, can set equal to VAct_mV
    float delta_floor_per_spike_mV = 1.0; // 0 means dynamic threshold floor not applied
    float tau_floor_decay_ms = 500.0;

    // Use directly from build_data
    //   UpdateMethod
    //   ResetMethod
    //   AfterHyperpolarizationSaturationModel
    //   AfterDepolarizationSaturationModel

public:
    // Parameters initialized by calculation in LIFCNeuron::LIFCNeuron()
    float tau_m_ms = 0.0;
    float g_L_nS = 0.0;
    float g_peak_ADP_max_nS = 0.0;

    float norm_fAHP = 0.0;
    float norm_sAHP = 0.0;
    float norm_ADP = 0.0;

    // Variables used during simulation but not initialized by parameters
    float g_fAHP_nS = 0.0;
    float g_sAHP_nS = 0.0;

    float fatigue = 0.0; // grows with spiking, decays slowly

    float a_ADP = 1.0; // ADP factor for resource depletion model
    float g_ADP_nS = 0.0;

    float h_spike = 1.0; // Adaptive threshold factor

    bool reset_done = true; // Only used for reset AFTER
    size_t updates_since_spike = 1000;
    float t_last_spike = -1000.0;

    float tDiff_ms = 0.0;

    bool abstracted_medians = false; // Done at start of simulation when build is complete

    std::vector<CoreStructs::LIFCReceptorData*> LIFCReceptorDataVec;
    std::vector<CoreStructs::LIFCReceptorData*> LIFCTransmitterDataVec;

public:
    LIFCNeuron(const CoreStructs::LIFCNeuronStruct & lifcneuronstruct, Simulation & _Sim);

    //! Returns the geometric center of the neuron.
    virtual Geometries::Vec3D& GetCellCenter();

    //! Update the assumed neuron "type" based on its neurotransmitters.
    virtual void UpdateType(Connections::NeurotransmitterType neurotransmitter);

    void spike(float t);
    void check_spiking(float t, float V_th_adaptive);
    void update_conductances(float t);
    float update_currents();
    float update_membrane_potential_forward_Euler(float I);
    void update_membrane_potential_exponential_Euler_Rm();
    void update_membrane_potential_exponential_Euler_Cm();
    float update_adaptive_threshold();
    void update_with_classical_reset_clamp(float t);
    void update_with_reset_options(float t);

    CoreStructs::LIFCReceptorData* FindLIFCReceptorPairing(LIFCNeuron* SrcNeuronPtr, Connections::LIFCReceptor* RPtr);
    void Show_Functional_Parameters();
    void Calculate_Abstracted_PSP_Medians();

    virtual void Update(float t_ms, bool recording);

    virtual void InputReceptorAdded(CoreStructs::LIFCReceptorData* RData);

    virtual void OutputTransmitterAdded(CoreStructs::LIFCReceptorData* RData);

    // Used in Simulation::GetConnectomeJSON().
    virtual void GetConnectomeTargetsJSON(nlohmann::json& targetvec, nlohmann::json& typevec, nlohmann::json& weightvec);

    // Used in Simulation::UpdatePrePostStrength().
    virtual bool UpdatePrePostStrength(int PresynapticID, float NewConductance_nS);

    // Pure abstraction count of number of synapses.
    virtual size_t GetAbstractConnection(int PreSynID, bool NonZero);

};

} // Simulator
} // NES
} // BG
