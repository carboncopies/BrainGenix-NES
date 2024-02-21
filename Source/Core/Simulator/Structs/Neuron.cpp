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

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
