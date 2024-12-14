//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the main simulation manager code.
    Additional Notes: None
    Date Created: 2024-02-13
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <memory>
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <assert.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>

#include <RPC/RPCManager.h>
#include <RPC/RPCHandlerHelper.h>

#include <Simulator/EngineController.h>
#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {


/**
 * @brief This class provides the infrastructure to run simulations.
 */
class NetmorphRPCInterface {

private:

    std::vector<std::unique_ptr<Simulation>>* Simulations_; /**Vector containing simulation instances. Index in this vector is the simulation's ID (Also stored in the simulation struct for reference.)*/

    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to the instance of the logging system*/


public:

    /**
     * @brief Construct a new Manager object
     * Give this a pointer to an initialized configuration object.
     * 
     * @param _Logger Pointer to the logging interface
     * @param _Config 
     * @param _RPCManager
     * @param _Renderer Instance of the rendering system.
     */
    NetmorphRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, std::vector<std::unique_ptr<Simulation>>* _Simulations, API::RPCManager* _RPCManager);

    /**
     * @brief Destroy the NetmorphRPCInterface object
     * 
     */
    ~NetmorphRPCInterface();

    /**
     * @brief Various routes for API
     * Note that since this file is getting too long, several RPC handlers are being set up - each taking over part of this.
     * See the relevant file in RPCInterface.(cpp/h) in various directories. 
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string NetmorphSetModelfile(std::string _JSONRequest);
    std::string NetmorphStartSimulation(std::string _JSONRequest);
    std::string NetmorphGetStatus(std::string _JSONRequest);
    std::string NetmorphGetFile(std::string _JSONRequest);
    std::string NetmorphSetLogBuffers(std::string _JSONRequest);
    std::string NetmorphGetLogBuffers(std::string _JSONRequest);

};

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
