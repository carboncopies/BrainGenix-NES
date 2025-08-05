//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Neuron abstract struct.
    Additional Notes: None
    Date Created: 2023-10-09
*/

#pragma once

#include <string>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <memory>
//#include <deque>

#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/Receptor.h>
#include <Simulator/Structs/BS.h>
#include <Simulator/Structs/SC.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

// Forward declarations:
struct Neuron;

struct ReceptorDataBase {
    int SrcNeuronID = -1;
    int DstNeuronID = -1;
    Neuron * SrcNeuronPtr = nullptr; // *** May want to get away from using these pointere and just use the Neuron IDs instead.
    Neuron * DstNeuronPtr = nullptr;

    ReceptorDataBase(Neuron * _SNPtr, Neuron * _DNPtr);
};

//! The neuron maintains informaition about receptors that were created.
struct ReceptorData: public ReceptorDataBase {
    int ReceptorID = -1;
    Connections::Receptor * ReceptorPtr = nullptr;

    ReceptorData(int _RID, Connections::Receptor * _RPtr, Neuron * _SNPtr, Neuron * _DNPtr);
};

//! This is used to store and update abstracted functional data for each
//! receptor type by pre-post neuron pair.
//! Note that this is quite different than the one-to-one mapping done in BS/SC ReceptorData.
struct LIFCReceptorData: public ReceptorDataBase {
    std::vector<int> ReceptorIDs;
    std::vector<Connections::LIFCReceptor*> ReceptorPtrs;

    float g_peak_sum_nS = 0.0; // used as abstracted g_peak_nS (sum)
    float weight_g_peak_sum = 0.0; // only used for abstracted weight recalculation
    float weight = 0.0; // abstracted weight
    float tau_rise_ms = 0.0; // abstracted tau_rise (median)
    float tau_decay_ms = 0.0; // abstracted tau_decay (median)
    float onset_delay_ms = 0.0; // abstracted onset_delay (median)

    float STDP_A_pos = 0.0; // abstracted A_pos (median)
    float STDP_A_neg = 0.0; // abstracted A_neg (median)
    float STDP_Tau_pos = 0.0; // abstracted tau_pos (median)
    float STDP_Tau_neg = 0.0; // abstracted tau_neg (median)

    float norm = 0.0; // Calculated once abstracted tau_rise_ms and tau_decay_ms are available
    float g_k = 0.0; // Calculated in Update_Conductance

    // A new LIFCReceptorData is created only where the data cannot be
    // added to an existing abstracted functional receptor connection.
    LIFCReceptorData(int _RID, Connections::LIFCReceptor * _RPtr, Neuron * _SNPtr, Neuron * _DNPtr);

    void AddToAbstractedFunctional(int _RID, Connections::LIFCReceptor * _RPtr);

    void Calculate_Abstracted_PSP_Medians();

    float E_k();
    bool voltage_gated();

    float g() { return g_k; }
    float gE_k() { return g_k * E_k(); }

    float Get_Current(float Vm) { return g_k * (Vm - E_k()); }

    Connections::NeurotransmitterType Type();

    Connections::LIFCSTDPMethodEnum STDP_Method();

    void Update_Conductance(float t, float Vm);

    void STDP_Update(float tfire);

    std::string Show_Functional_Parameters();

};

enum NeuronType: int {
    UnknownNeuron = 0,
    GenericPrincipalNeuron = 1,
    GenericInterneuron = 2,
    NUMNeuronType
};

enum NeuronClass: int {
    _Neuron = 0,
    _BSNeuron = 1,
    _SCNeuron = 2,
    _LIFCNeuron = 3,
    NUMNeuronClass
};

const std::map<std::string, NeuronType> Neurotransmitter2NeuronType = {
    { "AMPA", GenericPrincipalNeuron },
    { "NMDA", GenericPrincipalNeuron },
    { "GABA", GenericInterneuron }
};

/**
 * @brief This struct provides the base struct for all neurons.
 *
 */
struct Neuron {

    std::string Name; /**Name of the Neuron*/
    int ID; /**ID of the Neuron */

    NeuronType Type_ = UnknownNeuron;
    NeuronClass Class_ = _Neuron;

    Geometries::Vec3D cell_center; // *** FIX THIS!

    std::vector<float> TAct_ms{}; // Record of spike times
    std::vector<float> TDirectStim_ms{}; // was deque
    size_t next_directstim_idx = 0;

    bool is_first_update = true;

    //! Virtual destructor is essential for proper cleanup when
    //! Simulation::Neurons is destructed.
    virtual ~Neuron() {}

    int opto_wavelength_nm = 0;

    //! Update the assumed neuron "type" based on its neurotransmitters.
    virtual void UpdateType(const std::string & neurotransmitter);

    //! Returns the time since the action potential threshold was
    //! crossed last.
    virtual float DtAct_ms(float t_ms); // *** FIX THIS!

    //! Tells if the action potential threshold has been crossed.
    virtual bool HasSpiked(); // *** FIX THIS!

    //! Returns the geometric center of the neuron.
    virtual Geometries::Vec3D &GetCellCenter(); // *** FIX THIS!

    virtual void AddSpecificAPTime(float t_ms);

    unsigned long NumSpikes() const { return TAct_ms.size(); }

    virtual void Update(float t_ms, bool recording);

    virtual nlohmann::json GetSpikeTimesJSON() const;

    virtual nlohmann::json GetRecordingJSON() const;

    virtual void SetSpontaneousActivity(float mean, float stdev, int Seed);

    virtual void InputReceptorAdded(ReceptorData* RData);

    virtual void OutputTransmitterAdded(ReceptorData* RData);
};

//! NeuronRecording is an unordered map containing data from simulation in a
//! neuron.
typedef std::unordered_map<std::string, std::vector<float>> NeuronRecording;

struct BSNeuronStruct {
    
    std::string Name; /**Name of the Neuron*/
    int ID = -1; /**ID of the Neuron */

    int SomaCompartmentID;
    int AxonCompartmentID;

    float MembranePotential_mV;
    float RestingPotential_mV;
    float SpikeThreshold_mV;
    float DecayTime_ms;
    float AfterHyperpolarizationAmplitude_mV;
    float PostsynapticPotentialRiseTime_ms;
    float PostsynapticPotentialDecayTime_ms;
    float PostsynapticPotentialAmplitude_nA;

    // Direct access caches:
    Compartments::BS* SomaCompartmentPtr = nullptr;
    Compartments::BS* AxonCompartmentPtr = nullptr;

};

/**
 * @brief Crucial fixed-size data of a neuron. This is saved in neuronal
 * circuit model saves. Does not include name and cached data.
 */
struct SCNeuronBase {
    int ID = -1; /**ID of the Neuron */

    float MembranePotential_mV;
    float RestingPotential_mV;
    float SpikeThreshold_mV;
    float DecayTime_ms;
    float AfterHyperpolarizationAmplitude_mV;
    float PostsynapticPotentialRiseTime_ms;
    float PostsynapticPotentialDecayTime_ms;
    float PostsynapticPotentialAmplitude_nA;

    std::string str() const;
};

#define ADD_BYTES_TO_POINTER(theptr, numbytes) \
    (((uint8_t*)theptr)+numbytes)

struct SCNeuronStructFlatHeader {
    uint32_t FlatBufSize = 0;
    uint32_t NameSize = 0;
    uint32_t NameOffset = 0;
    uint32_t SomaCompartmentIDsSize = 0;
    uint32_t SomaCompartmentIDsOffset = 0;
    uint32_t DendriteCompartmentIDsSize = 0;
    uint32_t DendriteCompartmentIDsOffset = 0;
    uint32_t AxonCompartmentIDsSize = 0;
    uint32_t AxonCompartmentIDsOffset = 0;
    SCNeuronBase Base;

    std::string str() const;
    std::string name_str() const;
    std::string scid_str() const;
    std::string dcid_str() const;
    std::string acid_str() const;
};

struct SCNeuronStruct: public SCNeuronBase {
    
    std::string Name; /**Name of the Neuron*/

    std::vector<int> SomaCompartmentIDs;
    std::vector<int> DendriteCompartmentIDs;
    std::vector<int> AxonCompartmentIDs;

    std::unique_ptr<uint8_t[]> GetFlat() const;
    bool FromFlat(SCNeuronStructFlatHeader* header);

};

enum LIFCUpdateMethodEnum: int {
    EXPEULER_CM = 0, // Default: Exponential Euler Capacitance-based calculation
    EXPEULER_RM = 1,
    FORWARD_EULER = 2,
    CLASSICAL = 3,
    NUMLIFCUpdateMethodEnum = 4
};

enum LIFCResetMethodEnum: int {
    TOVM = 0,  // Default: Set to Vm prior to spike after absolute refractory period
    ONSET = 1, // Set to ResetPotential_mV at time step right after setting to SpikeDepolarization_mV
    AFTER = 2, // Set to ResetPotential_mV after absolute refractory period
    NUMLIFCResetMethodEnum = 3
};

enum LIFCAHPSaturationModelEnum: int {
    AHPCLIP = 0,    // Default: Clip at max cumulative level
    AHPSIGMOID = 1, // Sigmoidal saturation
    NUMLIFCAHPSaturationModelEnum = 2
};

enum LIFCADPSaturationModelEnum: int {
    ADPCLIP = 0,     // Default: Clip at max cumulative level
    ADPRESOURCE = 1, // Resource saturation
    NUMLIFCADPSaturationModelEnum  = 2
};

/**
 * @brief Crucial fixed-size data of a neuron. This is saved in neuronal
 * circuit model saves. Does not include name and cached data.
 */
struct LIFCNeuronBase {
    int ID = -1; /**ID of the Neuron */

    float RestingPotential_mV;
    float ResetPotential_mV;
    float SpikeThreshold_mV;
    float MembraneResistance_MOhm;
    float MembraneCapacitance_pF;
    float RefractoryPeriod_ms;
    float SpikeDepolarization_mV;

    LIFCUpdateMethodEnum UpdateMethod;
    LIFCResetMethodEnum ResetMethod;

    float AfterHyperpolarizationReversalPotential_mV;

    float FastAfterHyperpolarizationRise_ms;
    float FastAfterHyperpolarizationDecay_ms;
    float FastAfterHyperpolarizationPeakConductance_nS;
    float FastAfterHyperpolarizationMaxPeakConductance_nS;
    float FastAfterHyperpolarizationHalfActConstant;

    float SlowAfterHyperpolarizationRise_ms;
    float SlowAfterHyperpolarizationDecay_ms;
    float SlowAfterHyperpolarizationPeakConductance_nS;
    float SlowAfterHyperpolarizationMaxPeakConductance_nS;
    float SlowAfterHyperpolarizationHalfActConstant;

    LIFCAHPSaturationModelEnum AfterHyperpolarizationSaturationModel;

    float FatigueThreshold;
    float FatigueRecoveryTime_ms;

    float AfterDepolarizationReversalPotential_mV;
    float AfterDepolarizationRise_ms;
    float AfterDepolarizationDecay_ms;
    float AfterDepolarizationPeakConductance_nS;
    float AfterDepolarizationSaturationMultiplier;
    float AfterDepolarizationRecoveryTime_ms;
    float AfterDepolarizationDepletion;
    LIFCADPSaturationModelEnum AfterDepolarizationSaturationModel;

    float AdaptiveThresholdDiffPerSpike;
    float AdaptiveTresholdRecoveryTime_ms;
    float AdaptiveThresholdDiffPotential_mV;
    float AdaptiveThresholdFloor_mV;
    float AdaptiveThresholdFloorDeltaPerSpike_mV;
    float AdaptiveThresholdFloorRecoveryTime_ms;

    std::string str() const;
};

struct LIFCNeuronStructFlatHeader {
    uint32_t FlatBufSize = 0;
    uint32_t NameSize = 0;
    uint32_t NameOffset = 0;
    uint32_t SomaCompartmentIDsSize = 0;
    uint32_t SomaCompartmentIDsOffset = 0;
    uint32_t DendriteCompartmentIDsSize = 0;
    uint32_t DendriteCompartmentIDsOffset = 0;
    uint32_t AxonCompartmentIDsSize = 0;
    uint32_t AxonCompartmentIDsOffset = 0;
    LIFCNeuronBase Base;

    std::string str() const;
    std::string name_str() const;
    std::string scid_str() const;
    std::string dcid_str() const;
    std::string acid_str() const;
};

struct LIFCNeuronStruct: public LIFCNeuronBase {

    std::string Name; /**Name of the Neuron*/

    std::vector<int> SomaCompartmentIDs;
    std::vector<int> DendriteCompartmentIDs;
    std::vector<int> AxonCompartmentIDs;

    std::unique_ptr<uint8_t[]> GetFlat() const;
    bool FromFlat(LIFCNeuronStructFlatHeader* header);

};

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
