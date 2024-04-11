//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <fstream>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <cpp-base64/base64.cpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/RPCHandlerHelper.h>

#include <VSDA/VSDARPCInterface.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {



VSDARPCInterface::VSDARPCInterface(BG::Common::Logger::LoggingSystem* _Logger, API::RPCManager* _RPCManager, std::vector<std::unique_ptr<Simulation>>* _SimulationsVectorPointer) {

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
    _RPCManager->AddRoute("VSDA/EM/Initialize",           std::bind(&VSDARPCInterface::VSDAEMInitialize, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/EM/SetupMicroscope",      std::bind(&VSDARPCInterface::VSDAEMSetupMicroscope, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/EM/DefineScanRegion",     std::bind(&VSDARPCInterface::VSDAEMDefineScanRegion, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/EM/QueueRenderOperation", std::bind(&VSDARPCInterface::VSDAEMQueueRenderOperation, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/EM/GetRenderStatus",      std::bind(&VSDARPCInterface::VSDAEMGetRenderStatus, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/EM/GetImageStack",        std::bind(&VSDARPCInterface::VSDAEMGetImageStack, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/GetImage",                std::bind(&VSDARPCInterface::VSDAGetImage, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/Ca/Initialize",           std::bind(&VSDARPCInterface::VSDACAInitialize, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/Ca/SetupMicroscope",      std::bind(&VSDARPCInterface::VSDACASetupMicroscope, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/Ca/DefineScanRegion",     std::bind(&VSDARPCInterface::VSDACADefineScanRegion, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/Ca/QueueRenderOperation", std::bind(&VSDARPCInterface::VSDACAQueueRenderOperation, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/Ca/GetRenderStatus",      std::bind(&VSDARPCInterface::VSDACAGetRenderStatus, this, std::placeholders::_1));
    _RPCManager->AddRoute("VSDA/Ca/GetImageStack",        std::bind(&VSDARPCInterface::VSDACAGetImageStack, this, std::placeholders::_1));

}

VSDARPCInterface::~VSDARPCInterface() {

}

std::string VSDARPCInterface::VSDAEMInitialize(std::string _JSONRequest) {

    // Parse Request
    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/EM/Initialize", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();
    

    int Result = !VSDA_EM_Initialize(Logger_, ThisSimulation);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
    return ResponseJSON.dump();
}
std::string VSDARPCInterface::VSDAEMSetupMicroscope(std::string _JSONRequest) {

    // Parse Request, Get Parameters
    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/EM/SetupMicroscope", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();
    Logger_->Log(std::string("VSDA EM SetupMicroscope Called On Simulation With ID ") + std::to_string(ThisSimulation->ID), 4);

    // Build Microscope Paremters Data
    MicroscopeParameters Params;
    Handle.GetParFloat("PixelResolution_nm", Params.VoxelResolution_um);// This is not right, and we'll need to tweak some of these conversions, but for now it's the best we've got.
    Handle.GetParInt("ImageWidth_px", Params.ImageWidth_px);
    Handle.GetParInt("ImageHeight_px", Params.ImageHeight_px);
    Handle.GetParFloat("ScanRegionOverlap_percent", Params.ScanRegionOverlap_percent);
    Handle.GetParFloat("SliceThickness_nm", Params.SliceThickness_um);
    Handle.GetParFloat("MicroscopeFOV_deg", Params.MicroscopeFOV_deg);
    Handle.GetParInt("NumPixelsPerVoxel_px", Params.NumPixelsPerVoxel_px);

    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    int Result = !VSDA_EM_SetupMicroscope(Logger_, ThisSimulation, Params);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
    return ResponseJSON.dump();

}
std::string VSDARPCInterface::VSDAEMDefineScanRegion(std::string _JSONRequest) {

    // Parse Request, Get Parameters
    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/EM/DefineScanRegion", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();
    std::vector<float> Point1, Point2, Rotation_rad;
    Handle.GetParVecFloat("Point1_um", Point1);
    Handle.GetParVecFloat("Point2_um", Point2);
    Handle.GetParVecFloat("SampleRotation_rad", Rotation_rad);
    Logger_->Log(std::string("VSDA EM DefineScanRegion Called On Simulation With ID ") + std::to_string(ThisSimulation->ID), 4);

    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    if ((Point1.size() != 3) || (Point2.size() != 3) || (Rotation_rad.size() != 3)) {
        return Handle.ErrResponse();
    }

    // Setup Requested Bounding Box
    ScanRegion CreatedRegion;
    CreatedRegion.Point1X_um = Point1[0];
    CreatedRegion.Point1Y_um = Point1[1];
    CreatedRegion.Point1Z_um = Point1[2];

    CreatedRegion.Point2X_um = Point2[0];
    CreatedRegion.Point2Y_um = Point2[1];
    CreatedRegion.Point2Z_um = Point2[2];

    CreatedRegion.SampleRotationX_rad = Rotation_rad[0];
    CreatedRegion.SampleRotationY_rad = Rotation_rad[1];
    CreatedRegion.SampleRotationZ_rad = Rotation_rad[2];


    int ID = -1;
    int Result = !VSDA_EM_DefineScanRegion(Logger_, ThisSimulation, CreatedRegion, &ID);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
    ResponseJSON["ScanRegionID"] = ID;
    return ResponseJSON.dump();


}
std::string VSDARPCInterface::VSDAEMQueueRenderOperation(std::string _JSONRequest) {

    // Parse Request, Get Parameters
    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/EM/QueueRenderOperation", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();
    int ScanRegionID;
    Handle.GetParInt("ScanRegionID", ScanRegionID);
    Logger_->Log(std::string("VSDA EM QueueRenderOperation Called On Simulation With ID ") + std::to_string(ThisSimulation->ID), 4);

    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }


    // Check Region ID
    if (ScanRegionID < 0 || ScanRegionID >= ThisSimulation->VSDAData_.RenderedImagePaths_.size()) {
        Logger_->Log(std::string("VSDA EM GetImageStack Error, ScanRegion With ID ") + std::to_string(ScanRegionID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 3; // Error
        return ResponseJSON.dump();
    } 


    int Result = !VSDA_EM_QueueRenderOperation(Logger_, ThisSimulation, ScanRegionID);

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
    return ResponseJSON.dump();

}
std::string VSDARPCInterface::VSDAEMGetRenderStatus(std::string _JSONRequest) {


    // Parse Request, Get Parameters
    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/EM/GetRenderStatus", SimulationsPtr_, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0;
    ResponseJSON["RenderStatus"] = ThisSimulation->VSDAData_.State_;
    ResponseJSON["CurrentSlice"] = ThisSimulation->VSDAData_.CurrentSlice_;
    ResponseJSON["TotalSlices"] = ThisSimulation->VSDAData_.TotalSlices_;
    ResponseJSON["CurrentSliceImage"] = ThisSimulation->VSDAData_.CurrentSliceImage_;
    ResponseJSON["TotalSliceImages"] = ThisSimulation->VSDAData_.TotalSliceImages_;
    ResponseJSON["CurrentRegion"] = ThisSimulation->VSDAData_.CurrentRegion_;
    ResponseJSON["TotalRegions"] = ThisSimulation->VSDAData_.TotalRegions_;
    ResponseJSON["TotalImagesX"] = ThisSimulation->VSDAData_.TotalImagesX_;
    ResponseJSON["TotalImagesY"] = ThisSimulation->VSDAData_.TotalImagesY_;

    return ResponseJSON.dump();

}
std::string VSDARPCInterface::VSDAEMGetImageStack(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/EM/GetImageStack", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();
    int ScanRegionID;
    Handle.GetParInt("ScanRegionID", ScanRegionID);
    Logger_->Log(std::string("VSDA EM QueueRenderOperation Called On Simulation With ID ") + std::to_string(ThisSimulation->ID), 4);

    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }


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
    Logger_->Log(std::string("VSDA EM GetImageStack Called On Simulation With ID ") + std::to_string(ThisSimulation->ID) + ", Found " + std::to_string(ThisSimulation->VSDAData_.RenderedImagePaths_.size()) + " Layers", 4);
    ResponseJSON["RenderedImages"] = ImagePaths;


    return ResponseJSON.dump();


}
std::string VSDARPCInterface::VSDAGetImage(std::string _JSONRequest) {


    // Parse Request, Get Parameters
    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/GetImage", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();
    std::string ImageHandle;
    Handle.GetParString("ImageHandle", ImageHandle);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }



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



std::string VSDARPCInterface::VSDACAInitialize(std::string _JSONRequest) {

    // Parse Request
    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/CA/Initialize", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();
    

    int Result = !NES::VSDA::Calcium::VSDA_CA_Initialize(Logger_, ThisSimulation);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
    return ResponseJSON.dump();
}
std::string VSDARPCInterface::VSDACASetupMicroscope(std::string _JSONRequest) {


    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/Ca/SetupMicroscope", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Build Microscope Paremters Data
    NES::VSDA::Calcium::CaMicroscopeParameters Params;
    Handle.GetParVecInt("FlourescingNeuronIDs", Params.FlourescingNeuronIDs_);
    Handle.GetParString("CalciumIndicator", Params.CalciumIndicator_);
    Handle.GetParFloat("IndicatorRiseTime_ms", Params.IndicatorRiseTime_ms);
    Handle.GetParFloat("IndicatorDecayTime_ms", Params.IndicatorDecayTime_ms);
    Handle.GetParFloat("IndicatorInterval_ms", Params.IndicatorInterval_ms);
    Handle.GetParFloat("ImagingInterval_ms", Params.ImagingInterval_ms);
    Handle.GetParFloat("VoxelResolution_nm", Params.VoxelResolution_um);
    Handle.GetParFloat("ScanRegionOverlap_percent", Params.ScanRegionOverlap_percent);
    Handle.GetParInt("ImageWidth_px", Params.ImageWidth_px);
    Handle.GetParInt("ImageHeight_px", Params.ImageHeight_px);
    Handle.GetParInt("NumVoxelsPerSlice", Params.NumVoxelsPerSlice);
    Handle.GetParInt("NumPixelsPerVoxel_px", Params.NumPixelsPerVoxel_px);
    Handle.GetParFloat("BrightnessAmplification", Params.BrightnessAmplification);
    Handle.GetParFloat("AttenuationPerUm", Params.AttenuationPerUm);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    int Result = !NES::VSDA::Calcium::VSDA_CA_SetupMicroscope(Logger_, Handle.Sim(), Params);

    Handle.Sim()->CaData_.CaImaging.Init(Handle.Sim(), Handle.Sim()->CaData_.Params_);

    // Build Response
    return Handle.ErrResponse();

}
std::string VSDARPCInterface::VSDACADefineScanRegion(std::string _JSONRequest) {

    // Parse Request, Get Parameters
    // Parse Request, Get Parameters
    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/CA/DefineScanRegion", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();
    // Geometries::Vec3D Point1, Point2, SampleRotation;
    std::vector<float> Point1, Point2, Rotation_rad;
    Handle.GetParVecFloat("Point1_um", Point1);
    Handle.GetParVecFloat("Point2_um", Point2);
    Handle.GetParVecFloat("SampleRotation_rad", Rotation_rad);
    Logger_->Log(std::string("VSDA CA DefineScanRegion Called On Simulation With ID ") + std::to_string(ThisSimulation->ID), 4);

    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    if ((Point1.size() != 3) || (Point2.size() != 3) || (Rotation_rad.size() != 3)) {
        return Handle.ErrResponse();
    }

    // Setup Requested Bounding Box
    ScanRegion CreatedRegion;
    CreatedRegion.Point1X_um = Point1[0];
    CreatedRegion.Point1Y_um = Point1[1];
    CreatedRegion.Point1Z_um = Point1[2];

    CreatedRegion.Point2X_um = Point2[0];
    CreatedRegion.Point2Y_um = Point2[1];
    CreatedRegion.Point2Z_um = Point2[2];

    CreatedRegion.SampleRotationX_rad = Rotation_rad[0];
    CreatedRegion.SampleRotationY_rad = Rotation_rad[1];
    CreatedRegion.SampleRotationZ_rad = Rotation_rad[2];



    int ID = -1;
    int Result = !NES::VSDA::Calcium::VSDA_CA_DefineScanRegion(Logger_, ThisSimulation, CreatedRegion, &ID);


    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
    ResponseJSON["ScanRegionID"] = ID;
    return ResponseJSON.dump();


}
std::string VSDARPCInterface::VSDACAQueueRenderOperation(std::string _JSONRequest) {

    // Parse Request, Get Parameters
    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/CA/QueueRenderOperation", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();
    int ScanRegionID;
    Handle.GetParInt("ScanRegionID", ScanRegionID);
    Logger_->Log(std::string("VSDA CA QueueRenderOperation Called On Simulation With ID ") + std::to_string(ThisSimulation->ID), 4);

    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    int Result = !NES::VSDA::Calcium::VSDA_CA_QueueRenderOperation(Logger_, ThisSimulation, ScanRegionID);

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Result;
    return ResponseJSON.dump();

}
std::string VSDARPCInterface::VSDACAGetRenderStatus(std::string _JSONRequest) {


   // Parse Request, Get Parameters
    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/CA/GetRenderStatus", SimulationsPtr_, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = 0;
    ResponseJSON["RenderStatus"] = ThisSimulation->CaData_.State_;
    ResponseJSON["CurrentSlice"] = ThisSimulation->CaData_.CurrentSlice_;
    ResponseJSON["TotalSlices"] = ThisSimulation->CaData_.TotalSlices_;
    ResponseJSON["CurrentSliceImage"] = ThisSimulation->CaData_.CurrentSliceImage_;
    ResponseJSON["TotalSliceImages"] = ThisSimulation->CaData_.TotalSliceImages_;
    ResponseJSON["CurrentRegion"] = ThisSimulation->CaData_.CurrentRegion_;
    ResponseJSON["TotalRegions"] = ThisSimulation->CaData_.TotalRegions_;

    return ResponseJSON.dump();

}
std::string VSDARPCInterface::VSDACAGetImageStack(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "VSDA/CA/GetImageStack", SimulationsPtr_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
    Simulation* ThisSimulation = Handle.Sim();
    int ScanRegionID;
    Handle.GetParInt("ScanRegionID", ScanRegionID);
    Logger_->Log(std::string("VSDA EM QueueRenderOperation Called On Simulation With ID ") + std::to_string(ThisSimulation->ID), 4);

    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }


    // Check Region ID
    if (ScanRegionID < 0 || ScanRegionID >= ThisSimulation->CaData_.RenderedImagePaths_.size()) {
        Logger_->Log(std::string("VSDA Ca GetImageStack Error, ScanRegion With ID ") + std::to_string(ScanRegionID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 3; // Error
        return ResponseJSON.dump();
    } 


    // Build Response
    nlohmann::json ResponseJSON;

    ResponseJSON["StatusCode"] = ThisSimulation->CaData_.State_ != NES::VSDA::Calcium::CA_RENDER_DONE;

    nlohmann::json ImagePaths = ThisSimulation->CaData_.RenderedImagePaths_[ScanRegionID];
    Logger_->Log(std::string("VSDA Ca GetImageStack Called On Simulation With ID ") + std::to_string(ThisSimulation->ID) + ", Found " + std::to_string(ThisSimulation->VSDAData_.RenderedImagePaths_.size()) + " Layers", 4);
    ResponseJSON["RenderedImages"] = ImagePaths;


    return ResponseJSON.dump();


}


}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
