//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Neuron abstract struct.
    Additional Notes: None
    Date Created: 2024-01-13
*/

#include <Simulator/Structs/Neuron.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

//! Returns the time since the action potential threshold was
//! crossed last.
float Neuron::DtAct_ms(float t_ms) {
    return 0.0; // Placeholder.
};

//! Tells if the action potential threshold has been crossed.
bool Neuron::HasSpiked() {
    return false; // Placeholder.
}

//! Returns the geometric center of the neuron.
Geometries::Vec3D &Neuron::GetCellCenter() {
    return cell_center; // Placeholder.
}

void Neuron::AddSpecificAPTime(float t_ms) {
    AP_times_ms.emplace_back(t_ms);
}

void Neuron::Update(float t_ms, bool recording) {
    // Placeholder.
}


}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
