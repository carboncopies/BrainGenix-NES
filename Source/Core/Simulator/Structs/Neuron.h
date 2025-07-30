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

//! The neuron maintains informaition about receptors that were created.
struct ReceptorData {
    int ReceptorID = -1;
    int SrcNeuronID = -1;
    int DstNeuronID = -1;
    Connections::Receptor * ReceptorPtr = nullptr;
    Neuron * SrcNeuronPtr = nullptr; // *** May want to get away from using these pointere and just use the Neuron IDs instead.
    Neuron * DstNeuronPtr = nullptr;

    ReceptorData(int _RID, Connections::Receptor * _RPtr, Neuron * _SNPtr, Neuron * _DNPtr);
};
//typedef std::tuple<std::shared_ptr<CoreStructs::Neuron>, float> ReceptorData;

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
    { "GABA", GenericInterneuron },
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

    virtual void InputReceptorAdded(ReceptorData RData);

    virtual void OutputTransmitterAdded(ReceptorData RData);
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

    std::string str() const {
        std::stringstream ss;
        ss << "ID: " << ID;
        ss << "\nMembranePotential_mV: " << MembranePotential_mV;
        ss << "\nRestingPotential_mV: " << RestingPotential_mV;
        ss << "\nSpikeThreshold_mV: " << SpikeThreshold_mV;
        ss << "\nDecayTime_ms: " << DecayTime_ms;
        ss << "\nAfterHyperpolarizationAmplitude_mV: " << AfterHyperpolarizationAmplitude_mV;
        ss << "\nPostsynapticPotentialRiseTime_ms: " << PostsynapticPotentialRiseTime_ms;
        ss << "\nPostsynapticPotentialDecayTime_ms: " << PostsynapticPotentialDecayTime_ms;
        ss << "\nPostsynapticPotentialAmplitude_nA: " << PostsynapticPotentialAmplitude_nA << '\n';
        return ss.str();
    }
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

    std::string str() const {
        std::stringstream ss;
        ss << "FlatBufSize: " << FlatBufSize;
        ss << "\nNameSize: " << NameSize;
        ss << "\nNameOffset: " << NameOffset;
        ss << "\nSomaCompartmentIDsSize: " << SomaCompartmentIDsSize;
        ss << "\nSomaCompartmentIDsOffset: " << SomaCompartmentIDsOffset;
        ss << "\nDendriteCompartmentIDsSize: " << DendriteCompartmentIDsSize;
        ss << "\nDendriteCompartmentIDsOffset: " << DendriteCompartmentIDsOffset;
        ss << "\nAxonCompartmentIDsSize: " << AxonCompartmentIDsSize;
        ss << "\nAxonCompartmentIDsOffset: " << AxonCompartmentIDsOffset << '\n';
        ss << Base.str();
        return ss.str();
    }

    std::string name_str() const {
        std::stringstream ss;
        ss << "Name: " << (const char*) ADD_BYTES_TO_POINTER(this, NameOffset) << '\n';
        return ss.str();
    }

    std::string scid_str() const {
        std::stringstream ss;
        int* scidptr = (int*) ADD_BYTES_TO_POINTER(this, SomaCompartmentIDsOffset);
        //ss << "flat header address = " << uint64_t(this) << '\n';
        //ss << "scidptr = " << uint64_t(scidptr) << '\n';
        ss << "SomaCompartmentIDs: ";
        for (size_t idx = 0; idx < SomaCompartmentIDsSize; idx++) {
            ss << scidptr[idx] << ' ';
        }
        ss << '\n';
        return ss.str();
    }

    std::string dcid_str() const {
        std::stringstream ss;
        int* dcidptr = (int*) ADD_BYTES_TO_POINTER(this, DendriteCompartmentIDsOffset);
        ss << "DendriteCompartmentIDs: ";
        for (size_t idx = 0; idx < DendriteCompartmentIDsSize; idx++) {
            ss << dcidptr[idx] << ' ';
        }
        ss << '\n';
        return ss.str();
    }

    std::string acid_str() const {
        std::stringstream ss;
        int* acidptr = (int*) ADD_BYTES_TO_POINTER(this, AxonCompartmentIDsOffset);
        ss << "AxonCompartmentIDs: ";
        for (size_t idx = 0; idx < AxonCompartmentIDsSize; idx++) {
            ss << acidptr[idx] << ' ';
        }
        ss << '\n';
        return ss.str();
    }
};

struct SCNeuronStruct: public SCNeuronBase {
    
    std::string Name; /**Name of the Neuron*/

    std::vector<int> SomaCompartmentIDs;
    std::vector<int> DendriteCompartmentIDs;
    std::vector<int> AxonCompartmentIDs;

    // Direct access caches:
    // std::vector<Compartments::SC*> SomaCompartmentPtr;
    // std::vector<Compartments::SC*> DendriteCompartmentPtr;
    // std::vector<Compartments::SC*> AxonCompartmentPtr;

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
    NUMLIFCAHPSaturationModelEnum = 2,
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

    std::string str() const {
        std::stringstream ss;
        ss << "ID: " << ID;
        ss << "\nRestingPotential_mV: " << RestingPotential_mV;
        ss << "\nResetPotential_mV: " << ResetPotential_mV;
        ss << "\nSpikeThreshold_mV: " << SpikeThreshold_mV;
        ss << "\nMembraneResistance_MOhm: " << MembraneResistance_MOhm;
        ss << "\nMembraneCapacitance_pF: " << MembraneCapacitance_pF;
        ss << "\nRefractoryPeriod_ms: " << RefractoryPeriod_ms;
        ss << "\nSpikeDepolarization_mV: " << SpikeDepolarization_mV;
        ss << "\nUpdateMethod: " << UpdateMethod;
        ss << "\nResetMethod: " << ResetMethod;
        ss << "\nAfterHyperpolarizationReversalPotential_mV: " << AfterHyperpolarizationReversalPotential_mV;
        ss << "\nFastAfterHyperpolarizationRise_ms: " << FastAfterHyperpolarizationRise_ms;
        ss << "\nFastAfterHyperpolarizationDecay_ms: " << FastAfterHyperpolarizationDecay_ms;
        ss << "\nFastAfterHyperpolarizationPeakConductance_nS: " << FastAfterHyperpolarizationPeakConductance_nS;
        ss << "\nFastAfterHyperpolarizationMaxPeakConductance_nS: " << FastAfterHyperpolarizationMaxPeakConductance_nS;
        ss << "\nFastAfterHyperpolarizationHalfActConstant: " << FastAfterHyperpolarizationHalfActConstant;
        ss << "\nSlowAfterHyperpolarizationRise_ms: " << SlowAfterHyperpolarizationRise_ms;
        ss << "\nSlowAfterHyperpolarizationDecay_ms: " << SlowAfterHyperpolarizationDecay_ms;
        ss << "\nSlowAfterHyperpolarizationPeakConductance_nS: " << SlowAfterHyperpolarizationPeakConductance_nS;
        ss << "\nSlowAfterHyperpolarizationMaxPeakConductance_nS: " << SlowAfterHyperpolarizationMaxPeakConductance_nS;
        ss << "\nSlowAfterHyperpolarizationHalfActConstant: " << SlowAfterHyperpolarizationHalfActConstant;
        ss << "\nAfterHyperpolarizationSaturationModel: " << AfterHyperpolarizationSaturationModel;
        ss << "\nFatigueThreshold: " << FatigueThreshold;
        ss << "\nFatigueRecoveryTime_ms: " << FatigueRecoveryTime_ms;
        ss << "\nAfterDepolarizationReversalPotential_mV: " << AfterDepolarizationReversalPotential_mV;
        ss << "\nAfterDepolarizationRise_ms: " << AfterDepolarizationRise_ms;
        ss << "\nAfterDepolarizationDecay_ms: " << AfterDepolarizationDecay_ms;
        ss << "\nAfterDepolarizationPeakConductance_nS: " << AfterDepolarizationPeakConductance_nS;
        ss << "\nAfterDepolarizationSaturationMultiplier: " << AfterDepolarizationSaturationMultiplier;
        ss << "\nAfterDepolarizationRecoveryTime_ms: " << AfterDepolarizationRecoveryTime_ms;
        ss << "\nAfterDepolarizationDepletion: " << AfterDepolarizationDepletion;
        ss << "\nAfterDepolarizationSaturationModel: " << AfterDepolarizationSaturationModel;
        ss << "\nAdaptiveThresholdDiffPerSpike: " << AdaptiveThresholdDiffPerSpike;
        ss << "\nAdaptiveTresholdRecoveryTime_ms: " << AdaptiveTresholdRecoveryTime_ms;
        ss << "\nAdaptiveThresholdDiffPotential_mV: " << AdaptiveThresholdDiffPotential_mV;
        ss << "\nAdaptiveThresholdFloor_mV: " << AdaptiveThresholdFloor_mV;
        ss << "\nAdaptiveThresholdFloorDeltaPerSpike_mV: " << AdaptiveThresholdFloorDeltaPerSpike_mV;
        ss << "\nAdaptiveThresholdFloorRecoveryTime_ms: " << AdaptiveThresholdFloorRecoveryTime_ms << '\n';
        return ss.str();
    }
};

struct LIFCNeuronStruct: public LIFCNeuronBase {

    std::string Name; /**Name of the Neuron*/

    std::vector<int> SomaCompartmentIDs;
    std::vector<int> DendriteCompartmentIDs;
    std::vector<int> AxonCompartmentIDs;

    std::unique_ptr<uint8_t[]> GetFlat() const;
    bool FromFlat(SCNeuronStructFlatHeader* header);

};

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
