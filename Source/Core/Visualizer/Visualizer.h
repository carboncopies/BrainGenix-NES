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
#include <cstdint>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>
#include <BG/Renderer/Interface.h>
#include <Simulator/Structs/Simulation.h>

#include <Visualizer/MeshBuilder.h>
#include <Visualizer/MeshRenderer.h>
#include <Visualizer/VisualizerParameters.h>
#include <Visualizer/ImageProcessorPool/ImageProcessorPool.h>


namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief Function that renders a simulation for the user.
 * Takes a pointer to the simulation, along with some other parameters and renders what they've asked for to a file, which the path is returned to the _OutputPath parameter.
 *
 * @param _Logger Pointer to logging instance.
 * @param _Renderer Pointer to renderer instance.
 * @param _Simulation Pointer to simulation insance.
 * @return true
 * @return false
 */
bool VisualizeSimulation(BG::Common::Logger::LoggingSystem *_Logger, Renderer::Interface *_Renderer, Simulation *_Simulation, Visualizer::ImageProcessorPool* _ImageProcessorPool);

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
