//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides a service for querying other backend services.
    Additional Notes: Incomplete as of 2025-12-24
    Date Created: 2025-12-24
*/

#pragma once

// Standard Libraries
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

//Internal Libraries
#include <BG/Common/Logger/Logger.h>
#include <RPC/SafeClient.h>

class BackendServiceCommunicator {
    private:
        BG::SafeClient* APIClient_ = nullptr; /**Pointer to SafeClient instance for API communication*/
        BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to the logging system */
    
    public:
        /**
            * @brief Construct a new BackendServiceCommunicator object
            * 
            * @param _APIClient Pointer to SafeClient instance (must be initialized and connected to API)
            * @param _Logger Pointer to logging system instance
        */
        BackendServiceCommunicator(BG::SafeClient* _APIClient, BG::Common::Logger::LoggingSystem* _Logger);

        /** 
            * @brief Destroying the BackendServiceCommunicator Object
        */
        ~BackendServiceCommunicator();

        /**
            * @brief Query another backend service through the BrainGenix-API
            * 
            * @param _BackendService Name of the backend service to request from ("NES", "VSDA", "EVM", "ERS")
            * @param _RPCQuery Name of the RPC function/route to call on the target service
            * @param _QueryContent JSON string containing the query parameters/content
            * 
            * @return std::string JSON response from the target service, or error response on failure
        */
        std::string QueryOtherBackendServiceCommunicator(const std::string& _BackendService, const std::string& _RPCQuery, const std::string& _QueryContent);

};
