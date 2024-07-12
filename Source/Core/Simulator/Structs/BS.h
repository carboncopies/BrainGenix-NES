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
#include <sstream>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/Geometry.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Compartments {

/**
 * @brief Crucial fixed-size data of a component. This is saved in neuronal
 * circuit model saves. Does not include name and cached data.
 */
struct BSBaseData {
    int ID = -1; /**ID of the BS compartment*/

    int ShapeID; /**ID of the associated shape of this compartment*/

    float MembranePotential_mV; /**Membrane potential in millivolts*/
    float SpikeThreshold_mV; /**Action potential spike threshold voltage in millivolts*/
    float DecayTime_ms; /**After hyperpolarization time constant in ms, also known as tau_ahp*/
    float RestingPotential_mV; /**Resting Potential in millivolts*/
    float AfterHyperpolarizationAmplitude_mV; /**AfterHyperpolarization Amplitude in millivolts*/

    std::string str() const {
        std::stringstream ss;
        ss << "ID: " << ID;
        ss << "\nShapeID: " << ShapeID;
        ss << "\nMembranePotential_mV: " << MembranePotential_mV;
        ss << "\nSpikeThreshold_mV: " << SpikeThreshold_mV;
        ss << "\nDecayTime_ms: " << DecayTime_ms;
        ss << "\nRestingPotential_mV: " << RestingPotential_mV;
        ss << "\nAfterHyperpolarizationAmplitude_mV: " << AfterHyperpolarizationAmplitude_mV << '\n';
        return ss.str();
    }
};

/**
 * @brief This struct provides the data storage for each of the compartments
 * 
 */
struct BS: public BSBaseData {

    std::string Name; /**Name of the BS Compartment*/

    // Direct access caches:
    Geometries::Geometry* ShapePtr = nullptr;

    BS() {}
    BS(const BSBaseData& _Base): BSBaseData(_Base) {}

};

}; // Close Namespace Compartments
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
