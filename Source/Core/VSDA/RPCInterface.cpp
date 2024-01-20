#include <fstream>
#include <cpp-base64/base64.cpp>

#include <VSDA/RPCInterface.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {



RPCInterface::RPCInterface(BG::Common::Logger::LoggingSystem* _Logger, API::Manager* _RPCManager, std::vector<std::unique_ptr<Simulation>>* _SimulationsVectorPointer) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_RPCManager != nullptr);
    assert(_SimulationsVectorPointer != nullptr);

    // Copy Parameters To Member Variables
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
    _RPCManager->AddRoute("VSDA/EM/GetImage", Logger_, [this](std::string RequestJSON){ return VSDAEMGetImage(RequestJSON);});


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
    

    int Result = !VSDA_EM_Initialize(Logger_, ThisSimulation);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
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
    float MicroscopeFOV_deg          = Util::GetFloat(&RequestJSON, "MicroscopeFOV_deg");
    int NumPixelsPerVoxel_px         = Util::GetInt(&RequestJSON, "NumPixelsPerVoxel_px");
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
    Params.MicroscopeFOV_deg = MicroscopeFOV_deg;
    Params.NumPixelsPerVoxel_px = NumPixelsPerVoxel_px;

    int Result = !VSDA_EM_SetupMicroscope(Logger_, ThisSimulation, Params);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
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
    int Result = !VSDA_EM_DefineScanRegion(Logger_, ThisSimulation, CreatedRegion, &ID);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
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
    int Result = !VSDA_EM_QueueRenderOperation(Logger_, ThisSimulation, ScanRegionID);

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
    return ResponseJSON.dump();

}
std::string RPCInterface::VSDAEMGetRenderStatus(std::string _JSONRequest) {


    // Parse Request, Get Parameters
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");
    // Logger_->Log(std::string("VSDA EM GetRenderStatus Called On Simulation With ID ") + std::to_string(SimulationID), 0);

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
    ResponseJSON["CurrentSlice"] = ThisSimulation->VSDAData_.CurrentSlice_;
    ResponseJSON["TotalSlices"] = ThisSimulation->VSDAData_.TotalSlices_;
    ResponseJSON["CurrentSliceImage"] = ThisSimulation->VSDAData_.CurrentSliceImage_;
    ResponseJSON["TotalSliceImages"] = ThisSimulation->VSDAData_.TotalSliceImages_;
    
    return ResponseJSON.dump();

}
std::string RPCInterface::VSDAEMGetImageStack(std::string _JSONRequest) {

    // Parse Request, Get Parameters
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");
    int ScanRegionID = Util::GetInt(&RequestJSON, "ScanRegionID");
    Logger_->Log(std::string("VSDA EM GetImageStack Called On Simulation With ID ") + std::to_string(SimulationID), 4);

    // Check Sim ID
    if (SimulationID >= SimulationsPtr_->size() || SimulationID < 0) { // invlaid id
        Logger_->Log(std::string("VSDA EM GetImageStack Error, Simulation With ID ") + std::to_string(SimulationID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = (*SimulationsPtr_)[SimulationID].get();


    // Check Region ID
    if (ScanRegionID < 0 || ScanRegionID >= ThisSimulation->VSDAData_.RenderedImagePaths_.size()) {
        Logger_->Log(std::string("VSDA EM GetImageStack Error, ScanRegion With ID ") + std::to_string(ScanRegionID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 3; // Error
        return ResponseJSON.dump();
    } 


    // Build Response
    nlohmann::json ResponseJSON;

    ResponseJSON["StatusCode"] = ThisSimulation->VSDAData_.State_ != VSDA_RENDER_DONE;

    nlohmann::json ImagePaths = ThisSimulation->VSDAData_.RenderedImagePaths_[ScanRegionID];
    ResponseJSON["RenderedImages"] = ImagePaths;


    return ResponseJSON.dump();


}



std::string RPCInterface::VSDAEMGetImage(std::string _JSONRequest) {


    // Parse Request, Get Parameters
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");
    std::string ImageHandle = Util::GetString(&RequestJSON, "ImageHandle");
    // Logger_->Log(std::string("VSDA EM GetImage Called On Simulation ") + std::to_string(SimulationID) + ", Handle " + ImageHandle, 0);

    // Check Sim ID
    if (SimulationID >= SimulationsPtr_->size() || SimulationID < 0) { // invlaid id
        Logger_->Log(std::string("VSDA EM GetImage Error, Simulation With ID ") + std::to_string(SimulationID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = (*SimulationsPtr_)[SimulationID].get();


    // Minor security feature (probably still exploitable, so be warned!)
    // We just remove .. from the incoming handle for the image, since they're just files right now
    // as such, if we didnt strip that, then people could read any files on the server!
    // Also, we prepend a '.' so people can't try and get to the root
    std::string Pattern = "..";
    std::string::size_type i = ImageHandle.find(Pattern);
    while (i != std::string::npos) {
        Logger_->Log("Detected '..' In ImageHandle, It's Possible That Someone Is Trying To Do Something Nasty", 8);
        ImageHandle.erase(i, Pattern.length());
        i = ImageHandle.find(Pattern, i);
    }
    std::string SafeHandle = "./" + ImageHandle;


    // Now Check If The Handle Is Valid, If So, Load It
    std::ifstream ImageStream(SafeHandle.c_str(), std::ios::binary);
    std::string RawData;
    if (ImageStream.good()) {
        std::stringstream Buffer;
        Buffer << ImageStream.rdbuf();
        RawData = Buffer.str();
        // ImageStream>>RawData;
        ImageStream.close();

    } else {
        Logger_->Log("An Invalid ImageHandle Was Provided " + SafeHandle, 6);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 2; // error
        return ResponseJSON.dump();
    }

    // Now, Convert It To Base64
    std::string Base64Data = base64_encode(reinterpret_cast<const unsigned char*>(RawData.c_str()), RawData.length());


    // Build Response
    nlohmann::json ResponseJSON;

    ResponseJSON["StatusCode"] = (int)(ThisSimulation->VSDAData_.State_ != VSDA_RENDER_DONE);
    ResponseJSON["ImageData"] = Base64Data;

    // nlohmann::json ImagePaths = ThisSimulation->VSDAData_.RenderedImagePaths_[ScanRegionID];
    // ResponseJSON["RenderedImages"] = ImagePaths;


    return ResponseJSON.dump();

}






}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
