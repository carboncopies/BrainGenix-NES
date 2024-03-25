
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <cpp-base64/base64.h>



#include <Visualizer/VisualizerRPCInterface.h>
#include <RPC/APIStatusCode.h>


namespace BG {
namespace NES {
namespace Simulator {


VisualizerRPCInterface::VisualizerRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, std::vector<std::unique_ptr<Simulation>>* _Simulations, API::RPCManager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_RPCManager != nullptr);

    Logger_ = _Logger;
    Simulations_ = _Simulations;

    // Register Callbacks
    _RPCManager->AddRoute("Visualizer/GetStatus",        std::bind(&VisualizerRPCInterface::VisualizerGetStatus, this, std::placeholders::_1));
    _RPCManager->AddRoute("Visualizer/GetImageHandles",  std::bind(&VisualizerRPCInterface::VisualizerGetImageHandles, this, std::placeholders::_1));
    _RPCManager->AddRoute("Visualizer/GetImage",         std::bind(&VisualizerRPCInterface::VisualizerGetImage, this, std::placeholders::_1));
    _RPCManager->AddRoute("Visualizer/GenerateImages",   std::bind(&VisualizerRPCInterface::VisualizerGenerateImages, this, std::placeholders::_1));


}

VisualizerRPCInterface::~VisualizerRPCInterface() {

}

/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 * }
 */
std::string VisualizerRPCInterface::VisualizerGetStatus(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Visualizer/GetStatus", Simulations_, true);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    // Return Result ID
    return Handle.ResponseWithID("VisualizerStatus", Handle.Sim()->VisualizerParams.State); // ok
}


/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 * }
 */
std::string VisualizerRPCInterface::VisualizerGetImageHandles(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Visualizer/GetImageHandles", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }
  
    // Return Result ID
    std::string ImageHandleStrings = nlohmann::json(Handle.Sim()->VisualizerParams.FileHandles).dump();
    return Handle.StringResponse("ImageHandles", ImageHandleStrings); // ok
}


/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "ImageHandle": <str>
 * }
 */
std::string VisualizerRPCInterface::VisualizerGetImage(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Visualizer/GetImage", Simulations_);
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

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


    // Return Result ID
    return Handle.StringResponse("ImageData", Base64Data); // ok
}

/**
 * Expects _JSONRequest:
 * {
 *   "SimulationID": <SimID>,
 *   "Locations": [
 *     {
 *          "CameraPositionX_um": float,
 *          "CameraPositionY_um": float,
 *          "CameraPositionz_um": float,
 *          "CameraLookAtPositionX_um": float,
 *          "CameraLookAtPositionY_um": float,
 *          "CameraLookAtPositionz_um": float,
 *          "CameraFOV_deg": float
 *      },
 *      ...
 *      ]
 *   "ImageWidth_px": unsigned int,
 *   "ImageHeight_px": unsigned int
 * }
 */
std::string VisualizerRPCInterface::VisualizerGenerateImages(std::string _JSONRequest) {

    API::HandlerData Handle(_JSONRequest, Logger_, "Visualizer/GenerateImages", Simulations_);

  
    // Create and Populate Parameters From Request
    nlohmann::json::iterator LocationIterator;
    if (!Handle.FindPar("Locations", LocationIterator)) {
        return Handle.ErrResponse();
    }

    for (auto& ThisLocation: LocationIterator.value()) {
        Geometries::Vec3D Position;
        Handle.GetParVec3FromJSON("CameraPosition", Position, ThisLocation);
        Handle.Sim()->VisualizerParams.CameraPositionList_um.push_back(vsg::dvec3(Position.x, Position.y, Position.z));

        Geometries::Vec3D LookAtPosition;
        Handle.GetParVec3FromJSON("CameraLookAtPosition", Position, ThisLocation);
        Handle.Sim()->VisualizerParams.CameraLookAtPositionList_um.push_back(vsg::dvec3(LookAtPosition.x, LookAtPosition.y, LookAtPosition.z));

        float FOV_deg;
        Handle.GetParFloat("CameraFOV_deg", FOV_deg, ThisLocation);
        Handle.Sim()->VisualizerParams.FOVList_deg.push_back(FOV_deg);
    }

    Handle.Sim()->VisualizerParams.VisualizeElectrodes = true;          // Force the electrodes to be visualized for now -------------------------------------------

    Handle.GetParInt("ImageWidth_px", Handle.Sim()->VisualizerParams.ImageWidth_px);
    Handle.GetParInt("ImageHeight_px", Handle.Sim()->VisualizerParams.ImageHeight_px);


    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    Handle.Sim()->CurrentTask = SIMULATION_VISUALIZATION;
    Handle.Sim()->VisualizerParams.State = VISUALIZER_REQUESTED;
    Handle.Sim()->WorkRequested = true;


    // Return Result ID
    return Handle.ErrResponse(); // ok
}



}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
