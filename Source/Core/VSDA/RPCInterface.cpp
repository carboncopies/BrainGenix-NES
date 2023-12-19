#include <VSDA/RPCInterface.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {



RPCInterface::RPCInterface(BG::Common::Logger::LoggingSystem* _Logger, API::Manager* _RPCManager, BG::NES::Renderer::Interface* _Renderer, std::vector<std::unique_ptr<Simulation>>* _SimulationsVectorPointer) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_RPCManager != nullptr);
    assert(_Renderer != nullptr);
    assert(_SimulationsVectorPointer != nullptr);

    // Copy Parameters To Member Variables
    Renderer_ = _Renderer;
    Logger_ = _Logger;
    SimulationsPtr_ = _SimulationsVectorPointer;


    // Log Initialization
    Logger_->Log("Initializing RPC Interface for VSDA Subsystem", 4);

    // Register Callback For CreateSim
    _RPCManager->AddRoute("VSDA/EM/Initialize", Logger_, [this](std::string RequestJSON){ return VSDAEMInitialize(RequestJSON);});
    _RPCManager->AddRoute("VSDA/EM/SetupMicroscope", Logger_, [this](std::string RequestJSON){ return VSDAEMSetupMicroscope(RequestJSON);});
    _RPCManager->AddRoute("VSDA/EM/DefineScanRegion", Logger_, [this](std::string RequestJSON){ return VSDAEMDefineScanRegion(RequestJSON);});
    _RPCManager->AddRoute("VSDA/EM/QueueRenderOperation", Logger_, [this](std::string RequestJSON){ return VSDAEMQueueRenderOperation(RequestJSON);});
    _RPCManager->AddRoute("VSDA/EM/GetRenderStatus", Logger_, [this](std::string RequestJSON){ return VSDAEMGetRenderStatus(RequestJSON);});
    _RPCManager->AddRoute("VSDA/EM/GetImageStack", Logger_, [this](std::string RequestJSON){ return VSDAEMGetImageStack(RequestJSON);});


}

RPCInterface::~RPCInterface() {

}


std::string RPCInterface::VSDAEMInitialize(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");
    Logger_->Log(std::string("VSDA EM Initialize Called On Simulation With ID ") + std::to_string(SimulationID), 4);

    // Check Sim ID
    if (SimulationID >= SimulationsPtr_->size() || SimulationID < 0) { // invlaid id
        Logger_->Log(std::string("VSDA EM Initialize Error, Simulation With ID ") + std::to_string(SimulationID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = (*SimulationsPtr_)[SimulationID].get();
    

    int Status = !VSDA_EM_Initialize(Logger_, ThisSimulation);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Status;
    return ResponseJSON.dump();
}
std::string RPCInterface::VSDAEMSetupMicroscope(std::string _JSONRequest) {

    // Parse Request, Get Parameters
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID                 = Util::GetInt(&RequestJSON, "SimulationID");
    float PixelResolution_nm         = Util::GetFloat(&RequestJSON, "PixelResolution_nm");
    int ImageWidth_px                = Util::GetInt(&RequestJSON, "ImageWidth_px");
    int ImageHeight_px               = Util::GetInt(&RequestJSON, "ImageHeight_px");
    float SliceThickness_nm          = Util::GetFloat(&RequestJSON, "SliceThickness_nm");
    float ScanRegionOverlap_percent  = Util::GetFloat(&RequestJSON, "ScanRegionOverlap_percent");
    Logger_->Log(std::string("VSDA EM SetupMicroscope Called On Simulation With ID ") + std::to_string(SimulationID), 4);

    // Check Sim ID
    if (SimulationID >= SimulationsPtr_->size() || SimulationID < 0) { // invlaid id
        Logger_->Log(std::string("VSDA EM SetupMicroscope Error, Simulation With ID ") + std::to_string(SimulationID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = (*SimulationsPtr_)[SimulationID].get();
    

    // Build Microscope Paremters Data
    MicroscopeParameters Params;
    Params.VoxelResolution_um = PixelResolution_nm; // This is not right, and we'll need to tweak some of these conversions, but for now it's the best we've got.
    Params.ImageWidth_px = ImageWidth_px;
    Params.ImageHeight_px = ImageHeight_px;
    Params.ScanRegionOverlap_percent = ScanRegionOverlap_percent;
    Params.SliceThickness_um = SliceThickness_nm;

    int Status = !VSDA_EM_SetupMicroscope(Logger_, ThisSimulation, Params);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Status;
    return ResponseJSON.dump();

}
std::string RPCInterface::VSDAEMDefineScanRegion(std::string _JSONRequest) {

    // Parse Request, Get Parameters
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID                 = Util::GetInt(&RequestJSON, "SimulationID");
    Geometries::Vec3D Point1, Point2;
    Util::GetArrVec3(Point1, &RequestJSON, "Point1_um");
    Util::GetArrVec3(Point2, &RequestJSON, "Point2_um");
    Logger_->Log(std::string("VSDA EM DefineScanRegion Called On Simulation With ID ") + std::to_string(SimulationID), 4);

    // Check Sim ID
    if (SimulationID >= SimulationsPtr_->size() || SimulationID < 0) { // invlaid id
        Logger_->Log(std::string("VSDA EM DefineScanRegion Error, Simulation With ID ") + std::to_string(SimulationID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = (*SimulationsPtr_)[SimulationID].get();

    // Setup Requested Bounding Box
    ScanRegion CreatedRegion;
    CreatedRegion.Point1X_um = Point1.x;
    CreatedRegion.Point1Y_um = Point1.y;
    CreatedRegion.Point1Z_um = Point1.z;

    CreatedRegion.Point2X_um = Point2.x;
    CreatedRegion.Point2Y_um = Point2.y;
    CreatedRegion.Point2Z_um = Point2.z;


    int ID = -1;
    int Status = !VSDA_EM_DefineScanRegion(Logger_, ThisSimulation, CreatedRegion, &ID);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Status;
    ResponseJSON["ScanRegionID"] = ID;
    return ResponseJSON.dump();


}
std::string RPCInterface::VSDAEMQueueRenderOperation(std::string _JSONRequest) {

    // Parse Request, Get Parameters
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");
    int ScanRegionID = Util::GetInt(&RequestJSON, "ScanRegionID");
    Logger_->Log(std::string("VSDA EM QueueRenderOperation Called On Simulation With ID ") + std::to_string(SimulationID), 4);

    // Check Sim ID
    if (SimulationID >= SimulationsPtr_->size() || SimulationID < 0) { // invlaid id
        Logger_->Log(std::string("VSDA EM QueueRenderOperation Error, Simulation With ID ") + std::to_string(SimulationID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = (*SimulationsPtr_)[SimulationID].get();
    int Status = !VSDA_EM_QueueRenderOperation(Logger_, ThisSimulation, ScanRegionID);

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Status;
    return ResponseJSON.dump();

}
std::string RPCInterface::VSDAEMGetRenderStatus(std::string _JSONRequest) {


    // Parse Request, Get Parameters
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");
    Logger_->Log(std::string("VSDA EM GetRenderStatus Called On Simulation With ID ") + std::to_string(SimulationID), 4);

    // Check Sim ID
    if (SimulationID >= SimulationsPtr_->size() || SimulationID < 0) { // invlaid id
        Logger_->Log(std::string("VSDA EM GetRenderStatus Error, Simulation With ID ") + std::to_string(SimulationID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = (*SimulationsPtr_)[SimulationID].get();

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0;
    ResponseJSON["RenderStatus"] = ThisSimulation->VSDAData_.State_;
    return ResponseJSON.dump();

}
std::string RPCInterface::VSDAEMGetImageStack(std::string _JSONRequest) {

}






}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
