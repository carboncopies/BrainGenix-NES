//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the BS struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/Geometry.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Compartments {

/**
 * @brief This struct provides the data storage for each of the compartments
 * 
 */
struct BS {

    std::string Name; /**Name of the BS Compartment*/
    int ID = -1; /**ID of the BS compartment*/

    int ShapeID; /**ID of the associated shape of this compartment*/

    float MembranePotential_mV; /**Membrane potential in millivolts*/
    float SpikeThreshold_mV; /**Action potential spike threshold voltage in millivolts*/
    float DecayTime_ms; /**After hyperpolarization time constant in ms, also known as tau_ahp*/
    float RestingPotential_mV; /**Resting Potential in millivolts*/
    float AfterHyperpolarizationAmplitude_mV; /**AfterHyperpolarization Amplitude in millivolts*/

    // Direct access caches:
    Geometries::Geometry* ShapePtr = nullptr;

};

}; // Close Namespace Compartments
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
