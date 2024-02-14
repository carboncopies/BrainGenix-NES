//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")

#include <RPC/RPCManager.h>

#include <RPC/RPCHandlerHelper.h>
#include <RPC/APIStatusCode.h>


namespace BG {
namespace NES {
namespace API {


RPCManager::RPCManager(Config::Config* _Config, BG::Common::Logger::LoggingSystem* _Logger) {

    Logger_ = _Logger;

    // Initialize Server
    std::string ServerHost = _Config->Host;
    int ServerPort = _Config->PortNumber;

    // Create a unique pointer to the RPC server and initialize it with the provided port number
    RPCServer_ = std::make_unique<rpc::server>(ServerPort);

    // Register Basic Routes
    // Add predefined routes to the RPC server
    AddRoute("GetAPIVersion", _Logger, &GetAPIVersion);
    AddRoute("Echo", _Logger, &Echo);
    AddRoute("NES", Logger_, [this](std::string RequestJSON){ return NESRequest(RequestJSON);});
    

    int ThreadCount = std::thread::hardware_concurrency();
    _Logger->Log("Starting RPC Server With '" + std::to_string(ThreadCount) + "' Threads", 5);
    
    // Start the RPC server asynchronously with the specified thread count
    RPCServer_->async_run(ThreadCount);
}

RPCManager::~RPCManager() {
    // Destructor
    // No explicit cleanup needed as smart pointers manage the RPC server's memory
}


// void RPCManager::AddRequestHandler(std::string _RouteName, RouteAndHandler _Handler) {
    // RequestHandlers_.insert(std::pair<std::string, std::function<std::string>&>(_RouteName, _Handler));
// }

void RPCManager::AddRoute(std::string _RouteHandle, std::function<std::string(std::string _JSONRequest)> _Function) {
    Logger_->Log("Registering Callback For Route '" + _RouteHandle + "'", 4);
    RequestHandlers_.insert(std::pair<std::string, std::function<std::string(std::string _JSONRequest)>>(_RouteHandle, _Function));
    // RouteAndHandler Handler;
    // Handler.Route_ = _RouteHandle;
    // Handler.Handler_ = _Function;
    // AddRequestHandler(_RouteHandle, Handler);
}


bool BadReqID(int ReqID) {
    // *** TODO: Add some rules here for ReqIDs that should be refused.
    //           For example, keep track of the largest ReqID received
    //           and reject if ReqID is smaller than that. This is
    //           useful if ReqIDs are made using clock time or something.
    return false;
}



/**
 * This expects requests of the following format:
 * [
 *   {
 *     "ReqID": <request-id>,
 *     "AddBSNeuron": {
 *       "SimulationID": <SimID>, // Logically, this could belong outside AddBSNeuron, but we wish to reuse backward compatible functions. 
 *       "Name": <name>,
 *       "SomaID": <soma-id>,
 *       "AxonID": <axon-id>,
 *       <etc... all parameters>
 *     }
 *   },
 *   <more requests>
 * ]
 */
std::string RPCManager::NESRequest(std::string _JSONRequest, int _SimulationIDOverride) { // Generic JSON-based NES requests.

    // Parse Request
    //Logger_->Log(_JSONRequest, 3);
    API::HandlerData Handle(_JSONRequest, Logger_, "NES", nullptr, true, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    if (!Handle.ReqJSON().is_array()) {
        Logger_->Log("Bad format. Must be array of requests.", 8);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);       
    }

    // Build Response
    nlohmann::json ResponseJSON = nlohmann::json::array(); // Create empty array for the list of responses.

    // For each request in the JSON list:
    for (const auto& req : Handle.ReqJSON()) {

        int ReqID = -1;
        //int SimulationID = -1;
        std::string ReqFunc;
        nlohmann::json ReqParams;
        nlohmann::json ReqResponseJSON;
        //std::string Response;

        // Get the mandatory components of a request:
        for (const auto& [req_key, req_value]: req.items()) {
            if (req_key == "ReqID") {
                ReqID = req_value.template get<int>();
            //} else if (req_key == "SimID") {
            //    SimulationID = req_value.template get<int>();
            } else {
                ReqFunc = req_key;
                ReqParams = req_value;
            }
        }
        // if (BadReqID(ReqID)) { // e.g. < highest request ID already handled
        //     ReqResponseJSON["ReqID"] = ReqID;
        //     ReqResponseJSON["StatusCode"] = 1; // bad request id
        // } else {

        // Typically would call a specific handler from here, but let's just keep parsing.
        auto it = RequestHandlers_.find(ReqFunc);
        if (it == RequestHandlers_.end()) {
            Logger_->Log("Error, No Handler Exists For Call " + ReqFunc, 7);
            ReqResponseJSON["ReqID"] = ReqID;
            ReqResponseJSON["StatusCode"] = 1; // unknown request *** TODO: use the right code
            //Response = ReqResponseJSON.dump();
        } else {
            if (!it->second) {
                ReqResponseJSON["ReqID"] = ReqID;
                Logger_->Log("Error, Handler Is Null For Call " + ReqFunc + ", Continuing Anyway", 7);
                // ReqResponseJSON["StatusCode"] = 1; // not a valid NES request *** TODO: use the right code
            } else {
                Logger_->Log("DEBUG -> Got Request For '" + ReqFunc + "'", 0);
                if (_SimulationIDOverride != -1) {
                    ReqParams["SimulationID"] = _SimulationIDOverride;
                }
                std::string Response = it->second(ReqParams.dump()); // Calls the handler.
                // *** TODO: Either:
                //     a) Convert handlers to return nlohmann::json objects so that we
                //        can easily add ReqResponseJSON["ReqID"] = ReqID here, or,
                //     b) Convert Response back to a ReqResponseJSON here in order to
                //        add that... (This is more work, lower performance...)
                //     Right now, we do b) (sadly...)
                ReqResponseJSON = nlohmann::json::parse(Response);
                ReqResponseJSON["ReqID"] = ReqID;
            }
        }

        // }
        ResponseJSON.push_back(ReqResponseJSON);

    }

    std::string Response = ResponseJSON.dump();
    if (Response.length() < 1024) { 
        Logger_->Log("DEBUG --> Responding: " + Response, 0); // For DEBUG
    } else {
        Logger_->Log("DEBUG --> Response Omitted Due To Length", 0); // For DEBUG
    }
    // if (!IsLoadingSim()) {
    //     std::cout << "DEBUG ---> Responding: " << ResponseJSON.dump() << '\n'; std::cout.flush();
    // }

    return Handle.ResponseAndStoreRequest(ResponseJSON, false); // See comments at ResponseAndStoreRequest().
}



}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG