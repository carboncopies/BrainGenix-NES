//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the simulation engine code.
    Additional Notes: None
    Date Created: 2024-02-08
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <assert.h>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>
#include <BG/Renderer/Interface.h>
#include <Simulator/Structs/Simulation.h>

#include <VSDA/DebugHelpers/MeshConversionHelpers.h>




namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief This struct defines a bunch of parameters to be used by the VisualizeSimulation function.
 * It's just there to make it easier to call that function so we don't have a million params.
 * 
 */
struct VisualizerParameters {

    float FOV_deg; /**The desired field of view in degrees*/

    vsg::vec3 CameraPosition_um; /**Location of the camera in world-space coordinates*/
    vsg::vec3 CameraLookAtPosition_um; /**Location of where the camera is looking towards in world-space coordinates*/

    int ImageWidth_px; /**Target image width in pixels*/
    int ImageHeight_px; /**Target image height in pixels*/

}


/**
 * @brief Function that renders a simulation for the user. 
 * Takes a pointer to the simulation, along with some other parameters and renders what they've asked for to a file, which the path is returned to the _OutputPath parameter.
 * 
 * @param _Logger Pointer to logging instance.
 * @param _Renderer Pointer to renderer instance.
 * @param _Simulation Pointer to simulation insance.
 * @param _OutputPath Pointer to Output path string.
 * @return true 
 * @return false 
 */
bool VisualizeSimulation(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, Simulation* _Simulation, std::string* _OutputPath, VisualizerParameters* _Params);



}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG