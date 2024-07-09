#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <Netmorph/NetmorphRPCInterface.h>
#include <RPC/APIStatusCode.h>

#include <Netmorph/NetmorphManagerThread.h>


namespace BG {
namespace NES {
namespace Simulator {


NetmorphRPCInterface::NetmorphRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, std::vector<std::unique_ptr<Simulation>>* _Simulations, API::RPCManager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_RPCManager != nullptr);

    Logger_ = _Logger;
    Simulations_ = _Simulations;

    // Register Callbacks
    _RPCManager->AddRoute("Netmorph/SetModelfile",     std::bind(&NetmorphRPCInterface::NetmorphSetModelfile, this, std::placeholders::_1));
    _RPCManager->AddRoute("Netmorph/StartSimulation",  std::bind(&NetmorphRPCInterface::NetmorphStartSimulation, this, std::placeholders::_1));
    _RPCManager->AddRoute("Netmorph/GetStatus",        std::bind(&NetmorphRPCInterface::NetmorphGetStatus, this, std::placeholders::_1));


}

NetmorphRPCInterface::~NetmorphRPCInterface() {

}



/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "ModelFile": <string of model file>
 * }
 */
std::string NetmorphRPCInterface::NetmorphSetModelfile(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Netmorph/SetModelfile", Simulations_, true);

    std::string ModelFileString;
    Handle.GetParString("ModelFile", ModelFileString);
    
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }


    // Now Populate Modelfile
    Handle.Sim()->NetmorphParams.ModelFile = ModelFileString;


    // Return Result ID
    return Handle.ResponseWithID("NetmorphStatus", 0); // ok
}

/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>
 * }
 */
std::string NetmorphRPCInterface::NetmorphStartSimulation(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Netmorph/StartSimulation", Simulations_, true);

    
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }


    // Now Populate Modelfile
    int Status = 0;
    if (Handle.Sim()->NetmorphParams.State != Netmorph_WORKING && Handle.Sim()->NetmorphParams.State != Netmorph_REQUESTED) {
        Handle.Sim()->NetmorphParams.State = Netmorph_REQUESTED;
        Handle.Sim()->NetmorphParams.Progress_percent = 0;
    } else {
        Status = 3; // Something is broken
    }

    // Start Worker Thread
    Handle.Sim()->NetmorphParams.Sim = Handle.Sim();
    Logger_->Log("Starting Netmorph Worker Thread", 4);
    Handle.Sim()->NetmorphWorkerThread = std::thread(ExecuteNetmorphOperation, Logger_, &Handle.Sim()->NetmorphParams);

    // Return Result ID
    return Handle.ResponseWithID("NetmorphStatus", Status);
}



/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>
 * }
 */
std::string NetmorphRPCInterface::NetmorphGetStatus(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Netmorph/GetStatus", Simulations_, true);

    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return Result ID
    std::string Status = "None";
    if (Handle.Sim()->NetmorphParams.State == Netmorph_REQUESTED) {
        Status = "Requested";
    } else if (Handle.Sim()->NetmorphParams.State == Netmorph_WORKING) {
        Status = "Working";
    } else if (Handle.Sim()->NetmorphParams.State == Netmorph_DONE) {
        Status = "Done";
    }

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0;
    ResponseJSON["Progress_percent"] = Handle.Sim()->NetmorphParams.Progress_percent;
    ResponseJSON["NetmorphStatus"] = Status;
    return ResponseJSON.dump();

}



}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
