//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Implementation of BackendServiceCommunicator for querying other backend services.
    Additional Notes: Generic component that can be copied to all backend services
    Date Created: 2025-12-24
*/

// Standard Libraries
#include <string>
#include <vector>
#include <algorithm>

// Third-Party Libraries
#include <nlohmann/json.hpp>

// Internal Libraries
#include <RPC/BackendServiceCommunicator.h>



BackendServiceCommunicator::BackendServiceCommunicator(BG::SafeClient* _APIClient, BG::Common::Logger::LoggingSystem* _Logger) {
    APIClient_ = _APIClient;
    Logger_ = _Logger;

    if (!APIClient_) {
        if (Logger_) {
            Logger_->Log("BackendServiceCommunicator: Warning -- APIClient is null", 7);
        }
    }
}

BackendServiceCommunicator::~BackendServiceCommunicator() {
    // Deconstructor - no explicit cleanup needed since we don't own any pointers
}

std::string BackendServiceCommunicator::QueryOtherBackendServiceCommunicator(
    const std::string& _BackendService,
    const std::string& _RPCQuery,
    const std::string& _QueryContent) {


    // Validate that the APIClient exists first
    if (!APIClient_) {
        if (Logger_) {
            Logger_->Log("BackendServiceCommunicator: Warning -- BackendService is not provided or is null", 7);
        }
        nlohmann::json ErrorResponse;
        ErrorResponse["StatusCode"] = 1;
        ErrorResponse["ErrorMessage"] = "APIClient not initialized";
        return ErrorResponse.dump();
    }

    // Validate backend service names
    std::vector<std::string> ValidServices = {"NES", "VSDA", "EVM", "ERS"};

    // If the service was not found then log warning
    if (std::find(ValidServices.begin(), ValidServices.end(), _BackendService) == ValidServices.end()) {
        if (Logger_) {
            Logger_->Log("BackendServiceCommunicator: Invalid Service Name '" + _BackendService + "'", 7);
        }
        nlohmann::json ErrorResponse;
        ErrorResponse["StatusCode"] = 1;
        ErrorResponse["ErrorMessage"] = "Invalid Backend Service Name: " + _BackendService;
        return ErrorResponse.dump();
    }

    // Construction the JSON payload with nlohmann
    nlohmann::json RequestPayload;
    RequestPayload["TargetService"] = _BackendService;
    RequestPayload["RPCQuery"] = _RPCQuery;
    RequestPayload["QueryContent"] = _QueryContent;

    std::string RequestJSON = RequestPayload.dump();

    // Log the request with full details
    if (Logger_) {
        Logger_->Log("BackendServiceCommunicator: Querying " + _BackendService + " -> " + _RPCQuery, 4);
        Logger_->Log("BackendServiceCommunicator: Request JSON: " + RequestJSON, 3);
        Logger_->Log("BackendServiceCommunicator: Calling route 'QueryBackendService'", 3);
    }

    // Create the API Query
    // Use _ForceQuery = true to bypass health check and attempt the query anyway
    // This is necessary because the API service might not have GetAPIVersion route
    std::string ResponseJSON;
    bool Success = APIClient_->MakeJSONQuery("QueryBackendService", RequestJSON, &ResponseJSON, true);
    
    if (Logger_) {
        if (Success) {
            Logger_->Log("BackendServiceCommunicator: Got response: " + ResponseJSON, 3);
        } else {
            Logger_->Log("BackendServiceCommunicator: Query failed - no response received", 7);
        }
    }

    if (!Success) {
        if (Logger_) {
            Logger_->Log("BackendServiceCommunicator: Failed to query " + _BackendService + " -> " + _RPCQuery, 7);
        }
        nlohmann::json ErrorResponse;
        ErrorResponse["StatusCode"] = 1;
        ErrorResponse["ErrorMessage"] = "Failed to communicate with API service";
        return ErrorResponse.dump();
    }

    // Log the Success
    if (Logger_) {
        Logger_->Log("BackendServiceCommunicator: Successfully Queried: " + _BackendService + " -> " + _RPCQuery, 3);
    }

    return ResponseJSON;
}