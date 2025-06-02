//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the PatchClampDAC struct.
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
 * @brief Represents control data for the DAC.
 * 
 * This struct stores the time and corresponding voltage to be applied by the DAC.
 */
struct DACControlData {
    float AtTime_ms; /**< Time in milliseconds at which the voltage is applied. */
    float DACVoltage_mV; /**< Voltage in millivolts to be applied at the specified time. */

    /**
     * @brief Default constructor initializing time and voltage to zero.
     */
    DACControlData(): AtTime_ms(0.0), DACVoltage_mV(0.0) {}

    /**
     * @brief Parameterized constructor to initialize time and voltage.
     * 
     * @param t_ms Time in milliseconds.
     * @param v_mV Voltage in millivolts.
     */
    DACControlData(float t_ms, float v_mV): AtTime_ms(t_ms), DACVoltage_mV(v_mV) {}
};

/**
 * @brief This struct implements the storage state of the DACs.
 * 
 * The PatchClampDAC struct represents a digital-to-analog converter (DAC) used
 * in patch clamp experiments. It stores information about the DAC's configuration
 * and control data.
 */
struct PatchClampDAC {

    std::string Name; /**< Name of the DAC. */
    int ID; /**< ID of the DAC. */

    int DestinationCompartmentID; /**< ID of the compartment receiving DAC output. */

    Geometries::Vec3D ClampPos_um; /**< Position of the clamp in world space coordinates. */

    //! List of control data specifying time and voltage values.
    std::vector<DACControlData> ControlData;

};

}; // Close Namespace Tools
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
