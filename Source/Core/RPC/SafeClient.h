//=================================================================//
// This file is part of the BrainGenix-EVM Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides RPC management functionality for the system.
    Additional Notes: None
    Date Created: 2024-04-10
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <rpc/client.h>
#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/StaticRoutes.h>
#include <RPC/RouteAndHandler.h>

#include <BG/Common/Logger/Logger.h>

#include <Config/Config.h>



namespace BG {


class SafeClient {

private:

    std::unique_ptr<::rpc::client> Client_; /**Client to upstream NES Service*/
    std::atomic_bool IsHealthy_; /**Boolean indicating if the client is healhy or not*/
    std::atomic_bool RequestExit_; /**Indicates if the thread is to be terminated or not*/
    
    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to the instance of the logging system*/

    std::thread ClientManager_; /**Thread that owns the client, and autoreconnects, etc.*/

    bool LastState_ = false;



    std::string RPCHost_ = "";
    int RPCPort_ = -1;
    int RPCTimeout_ms = -1;


    void RPCManagerThread();
    bool RunVersionCheck();
    bool Connect();

public:

    /**
     * @brief Construct a new SafeClient object
     * Give this a pointer to an initialized configuration object.
     * Also it takes a pointer to the logging system instance.
     * 
     * @param _Config 
     * @param _Logger
     */
    SafeClient(BG::Common::Logger::LoggingSystem* _Logger);


    /**
     * @brief Destroy the SafeClient object
     * 
     */
    ~SafeClient();


    bool SetTimeout(int _Timeout_ms);
    bool SetHostPort(std::string _Host, int _Port);
    void Reconnect();

    bool MakeJSONQuery(std::string _Route, std::string* _Result, bool _ForceQuery = false);
    bool MakeJSONQuery(std::string _Route, std::string _Query, std::string* _Result, bool _ForceQuery = false);


};

}; // Close Namespace BG