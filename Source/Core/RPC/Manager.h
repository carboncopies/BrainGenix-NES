//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides RPC management functionality for the system.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <rpc/server.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/StaticRoutes.h>

#include <BG/Common/Logger/Logger.h>

#include <Config/Config.h>



namespace BG {
namespace NES {
namespace API {



/**
 * @brief Manages the NES remote procedure call (RPC) host.
 *
 * The Manager class owns the RPC server and handles its initialization and destruction at the end of the program's run.
 * It also takes a copy of the systemwide configuration struct to configure the RPC server (e.g., host and port).
 */
class Manager {

private:

    Config::Config* Config_; /**Pointer to configuration struct owned by rest of system*/
    std::unique_ptr<rpc::server> RPCServer_; /**Instance of RPC Server from rpclib*/

public:

    /**
     * @brief Construct a new Manager object
     * Give this a pointer to an initialized configuration object.
     * Also it takes a pointer to the logging system instance.
     * 
     * @param _Config 
     * @param _Logger
     */
    Manager(Config::Config* _Config, BG::Common::Logger::LoggingSystem* _Logger);


    /**
     * @brief Destroy the Manager object
     * 
     */
    ~Manager();


    /**
     * @brief Registers a callback to the API service.
     *
     * Registers a callback function for a specific route. Assumes the callback function may be accessed from any thread.
     *
     * @param _RouteName Name of the route to register.
     * @param _Logger Pointer to the logging system instance.
     * @param _CallbackFunction Callback function to be registered.
     */
    template <typename F> void AddRoute(std::string _RouteName, BG::Common::Logger::LoggingSystem* _Logger, F _CallbackFunction) {
        std::string LogMsg = "Registering Callback For Route " + _RouteName;
        _Logger->Log(LogMsg, 0);
        RPCServer_->bind(_RouteName.c_str(), _CallbackFunction);
    }


};

}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG