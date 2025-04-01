#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <Netmorph/NetmorphRPCInterface.h>
#include <RPC/APIStatusCode.h>
#include <cpp-base64/base64.h>
#include <uuid.h>

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
    _RPCManager->AddRoute("Netmorph/GetFile",          std::bind(&NetmorphRPCInterface::NetmorphGetFile, this, std::placeholders::_1));

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
    Handle.GetParString("ModelContent", ModelFileString);
    
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }


    // Now Populate Modelfile
    Handle.Sim()->NetmorphParams.ModelContent = ModelFileString;


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

    Handle.GetParString("ModelContent", Handle.Sim()->NetmorphParams.ModelContent);
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

    // Create a unique output directory for this Netmorph run
    uuids::uuid const ThisID = uuids::uuid_system_generator{}();
    std::string UUID = uuids::to_string(ThisID);
    std::string NetmorphOutputPath = "NetmorphOutput/" + UUID + "/";
    std::error_code err;
    err.clear();
    if (!std::filesystem::create_directories(NetmorphOutputPath, err)) {
        if (std::filesystem::exists(NetmorphOutputPath)) {
            // The folder already exists:
            err.clear();
        } else {
            return Handle.ErrResponse();
        }
    }
    Handle.Sim()->NetmorphParams.OutputDirectory = NetmorphOutputPath;

    // Start Worker Thread
    Handle.Sim()->NetmorphParams.Sim = Handle.Sim();
    Logger_->Log("Starting Netmorph Worker Thread", 4);
    Handle.Sim()->NetmorphWorkerThread = std::thread(ExecuteNetmorphOperation, Logger_, &Handle.Sim()->NetmorphParams);

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Handle.GetStatus();
    ResponseJSON["NetmorphOutputDirectory"] = Handle.Sim()->NetmorphParams.OutputDirectory;
    ResponseJSON["NetmorphStatus"] = Status;
    return Handle.ResponseAndStoreRequest(ResponseJSON);
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

/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "FileID": <FileID>
 * }
 * 
 * Returns:
 * {
 *   "StatusCode": <StatusCode>,
 *   "FileData": <base64-encoded-data>
 * }
 */
std::string NetmorphRPCInterface::NetmorphGetFile(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Netmorph/GetFile", Simulations_, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    std::string FileID;
    Handle.GetParString("FileID", FileID);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    std::string NetmorphOutputDirectory(Handle.Sim()->NetmorphParams.Result.net->get_outputdirectory().chars());
    std::string FilePath(NetmorphOutputDirectory+FileID);

    // Open the file, see if it is valid.
    std::ifstream NetmorphFile(FilePath.c_str(), std::ios::binary);
    if (!NetmorphFile.good()) {
        Logger_->Log("Invalid file "+FilePath, 6);
        return Handle.ErrResponse(API::BGStatusInvalidParametersPassed);
    }

    std::stringstream RawData;
    RawData << NetmorphFile.rdbuf();
    NetmorphFile.close();

    // Base64 encode the data.
    std::string Base64Data = base64_encode(reinterpret_cast<const unsigned char*>(RawData.str().c_str()), RawData.str().size());

    return Handle.StringResponse("FileData", Base64Data); // ok, with data

}

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
