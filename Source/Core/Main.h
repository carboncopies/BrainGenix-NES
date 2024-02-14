//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: 
        This header file serves as the main entry point for the BrainGenix-NES Neuron Emulation System.
        It includes essential libraries and sets up the main function for the system.

    Additional Notes: 
        None

    Date Created: 
        2023-06-24
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <functional>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <rpc/server.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>
#include <BG/Renderer/Interface.h>
#include <Config/ConfigurationManager.h>
#include <Config/Config.h>
#include <RPC/RPCManager.h>

#include <Simulator/RPC/SimulationRPCInterface.h>
#include <Simulator/RPC/GeometryRPCInterface.h>
#include <Simulator/RPC/ModelRPCInterface.h>
#include <VSDA/VSDARPCInterface.h>
#include <Visualizer/VisualizerRPCInterface.h>

#include <VSDA/RenderPool.h>
#include <Visualizer/VisualizerPool.h>
#include <Util/LogLogo.h>
#include <Profiling/ProfilingManager.h>

/**
 * @brief The main entry point for the BrainGenix-NES Neuron Emulation System.
 * 
 * This function initializes the system, sets up essential components, and starts the main loop.
 * 
 * @param NumArguments The number of command-line arguments.
 * @param ArgumentValues An array containing the command-line arguments.
 * @return int The exit code of the program.
 */
int main(int NumArguments, char** ArgumentValues);
