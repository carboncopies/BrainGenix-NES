//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Neuron abstract struct.
    Additional Notes: None
    Date Created: 2024-01-13
*/

#include <iostream>

#include <Simulator/Structs/Neuron.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

// Use this to warn about accidentally having reached the wrong
// OOP level:
#define WARNWRONGOOPLEVEL() {\
    std::cout << "DEBUG --> Wrong OOP level: " << __func__ << '\n'; std::cout.flush();\
}

ReceptorData::ReceptorData(int _RID, Connections::Receptor * _RPtr, Neuron * _SNPtr, Neuron * _DNPtr):
        ReceptorID(_RID), SrcNeuronID(_SNPtr->ID), DstNeuronID(_DNPtr->ID), ReceptorPtr(_RPtr), SrcNeuronPtr(_SNPtr), DstNeuronPtr(_DNPtr) {
}

//! Update the assumed neuron type based on its neurotransmitters.
void Neuron::UpdateType(const std::string & neurotransmitter) {
    // Only do this if it is still "unknown".
    if (Type_==UnknownNeuron) {
        auto it = Neurotransmitter2NeuronType.find(neurotransmitter);
        if (it != Neurotransmitter2NeuronType.end()) {
            Type_ = it->second;
        }
    }
}

//! Returns the time since the action potential threshold was
//! crossed last.
float Neuron::DtAct_ms(float t_ms) {
    WARNWRONGOOPLEVEL();
    return 0.0; // Placeholder.
};

//! Tells if the action potential threshold has been crossed.
bool Neuron::HasSpiked() {
    WARNWRONGOOPLEVEL();
    return false; // Placeholder.
}

//! Returns the geometric center of the neuron.
Geometries::Vec3D &Neuron::GetCellCenter() {
    WARNWRONGOOPLEVEL();
    return cell_center; // Placeholder.
}

void Neuron::AddSpecificAPTime(float t_ms) {
    TDirectStim_ms.emplace_back(t_ms);
}

void Neuron::Update(float t_ms, bool recording) {
    // Placeholder.
    WARNWRONGOOPLEVEL();
}

nlohmann::json Neuron::GetRecordingJSON() const {
    // Placeholder.
    nlohmann::json recording;
    WARNWRONGOOPLEVEL();
    return recording;
}

void Neuron::SetSpontaneousActivity(float mean, float stdev, int Seed) {
    WARNWRONGOOPLEVEL();
}

void Neuron::InputReceptorAdded(ReceptorData RData) {
    WARNWRONGOOPLEVEL();
}

void Neuron::OutputTransmitterAdded(ReceptorData RData) {
    WARNWRONGOOPLEVEL();
}

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
