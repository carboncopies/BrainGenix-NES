//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the simulation engine code.
    Additional Notes: None
    Date Created: 2023-06-29
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
 * @brief Will Build a scene containing a 3d representation of the simulation in the renderer instance provided.
 * Logs output via the Logger pointer, and returns true/false based on success/failure.
 * 
 * @param _Logger Pointer to logger instance
 * @param _Renderer Pointer to instance of NES renderer instance
 * @param _Simulation Pointer to valid simulation struct where data is read out of.
 * @return true 
 * @return false 
 */
bool BuildMeshFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, Simulation* _Simulation);

/**
 * @brief Will add visual representations of electrodes to the scene.
 * 
 * @param _Logger Pointer to logger instance
 * @param _Renderer Pointer to instance of NES renderer instance
 * @param _Simulation Pointer to valid simulation struct where data is read out of.
 * @return true 
 * @return false 
 */
bool BuildMeshFromElectrodes(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, Simulation* _Simulation);


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG