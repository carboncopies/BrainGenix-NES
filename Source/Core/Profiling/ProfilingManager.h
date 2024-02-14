//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the some profiling util functions.
    Additional Notes: None
    Date Created: 2024-01-20
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <memory>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <assert.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/RPC/SimulationRPCInterface.h>

#include <Config/Config.h>
#include <BG/Common/Logger/Logger.h>

#include <Visualizer/VisualizerPool.h>


#include <VSDA/RenderPool.h>

namespace BG {
namespace NES {
namespace Profiling {



/**
 * @brief Automatically runs profiling tests.
 * 
 * @param _Logger 
 * @param _Config 
 * @param _SimManager 
 * @param _RPCManager 
 */
int Manager(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, Simulator::SimulationRPCInterface* _SimManager, Simulator::VSDA::RenderPool* _RenderPool, Simulator::VisualizerPool* _VisualizerPool, API::RPCManager* _RPCManager);


}; // Close Namespace Profiling
}; // Close Namespace NES
}; // Close Namespace BG
