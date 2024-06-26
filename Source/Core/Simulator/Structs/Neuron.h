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
#include <tuple>
#include <unordered_map>
#include <vector>
#include <memory>
#include <deque>

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

const std::map<std::string, NeuronType> Neurotransmitter2NeuronType = {
    { "AMPA", GenericPrincipalNeuron },
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

    Geometries::Vec3D cell_center; // *** FIX THIS!

    std::vector<float> TAct_ms{};
    std::deque<float> TDirectStim_ms{};

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

struct SCNeuronStruct {
    
    std::string Name; /**Name of the Neuron*/
    int ID = -1; /**ID of the Neuron */

    std::vector<int> SomaCompartmentIDs;
    std::vector<int> DendriteCompartmentIDs;
    std::vector<int> AxonCompartmentIDs;

    float MembranePotential_mV;
    float RestingPotential_mV;
    float SpikeThreshold_mV;
    float DecayTime_ms;
    float AfterHyperpolarizationAmplitude_mV;
    float PostsynapticPotentialRiseTime_ms;
    float PostsynapticPotentialDecayTime_ms;
    float PostsynapticPotentialAmplitude_nA;

    // Direct access caches:
    // std::vector<Compartments::SC*> SomaCompartmentPtr;
    // std::vector<Compartments::SC*> DendriteCompartmentPtr;
    // std::vector<Compartments::SC*> AxonCompartmentPtr;

};

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
