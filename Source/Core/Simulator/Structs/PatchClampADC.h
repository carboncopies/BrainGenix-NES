//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the PatchClampADC struct.
    Additional Notes: None
    Date Created: 2023-06-28
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Tools {

/**
 * @brief This struct provides the data storage for the PatchClampADC
 * 
 */
struct PatchClampADC {

    std::string Name; /**Name of the ADC*/
    int ID; /**ID of this ADC*/

    int SourceCompartmentID; /**SourceCompartmentID ID of the compartment being read by the ADC output.*/
    Geometries::Vec3D ClampPos_um; /**Position of the clamp adc in world space, in micrometers*/

    float Timestep_ms; /**Recording Timestep for this patchclamp ADC*/

    std::vector<float> RecordedData_mV; /**Vector containing list of recorded data in milliseconds*/


};

}; // Close Namespace Tools
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
