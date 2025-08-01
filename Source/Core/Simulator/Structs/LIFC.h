//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the LIFC struct.
    Additional Notes: None
    Date Created: 2025-07-28
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/Geometry.h>
//#include <Simulator/Structs/SC.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Compartments {

/**
 * @brief Crucial fixed-size data of a component. This is saved in neuronal
 * circuit model saves. Does not include name and cached data.
 */
struct LIFCBaseData {
    int ID = -1; /**ID of the LIFC compartment*/

    int ShapeID; /**ID of the associated shape of this compartment*/

    float RestingPotential_mV; /**Resting potential in millivolts*/
    float ResetPotential_mV; /** Reset potential in millivolts*/
    float SpikeThreshold_mV; /**Action potential spike threshold voltage in millivolts*/
    float MembraneResistance_MOhm; /** Must be converted to giga-Ohm!*/
    float MembraneCapacitance_pF;
    float AfterHyperpolarizationAmplitude_mV; /**AfterHyperpolarization Amplitude in millivolts*/

    std::string str() const;
};

/**
 * @brief This struct provides the data storage for each of the compartments
 * 
 */
struct LIFC: public LIFCBaseData {

    std::string Name; /**Name of the LIFC Compartment*/

    // Direct access caches:
    Geometries::Geometry* ShapePtr = nullptr;

    LIFC() {}
    LIFC(const LIFCBaseData& _Base): LIFCBaseData(_Base) {}

};


}; // Close Namespace Compartments
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
