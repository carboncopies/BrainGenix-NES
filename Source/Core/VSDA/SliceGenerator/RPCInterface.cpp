#include <VSDA/SliceGenerator/RPCInterface.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {



std::string RPCInterface::VSDAEMInitialize(std::string _JSONRequest) {

    // Parse Request
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID = Util::GetInt(&RequestJSON, "SimulationID");
    Logger_->Log(std::string("VSDA EM Initialize Called On Simulation With ID ") + std::to_string(SimulationID), 4);

    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        Logger_->Log(std::string("VSDA EM Initialize Error, Simulation With ID ") + std::to_string(SimulationID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    

    int Status = !VSDAInitialize(Logger_, ThisSimulation);


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
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        Logger_->Log(std::string("VSDA EM SetupMicroscope Error, Simulation With ID ") + std::to_string(SimulationID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    

    // Build Microscope Paremters Data
    MicroscopeParameters Params;
    Params.VoxelResolution_um = PixelResolution_nm*1000; // This is not right, and we'll need to tweak some of these conversions, but for now it's the best we've got.
    Params.ImageWidth_px = ImageWidth_px;
    Params.ImageHeight_px = ImageHeight_px;
    Params.ScanRegionOverlap_percent = ScanRegionOverlap_percent;
    Params.SliceThickness_um = SliceThickness_nm*1000;

    int Status = !VSDASetupMicroscope(Logger_, ThisSimulation, Params);

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Status;
    return ResponseJSON.dump();

}
std::string RPCInterface::VSDAEMDefineScanRegion(std::string _JSONRequest) {

/**
- (bgSimulationID) `SimulationID` ID of simulation to setup the microscope for.  
- (vec3) `Point1_um` (X,Y,Z) World space location of one corner of the rectangular prism enclosing the target scan region.  
- (vec3) `Point2_um` (X,Y,Z) World space location of the other corner of the rectangular prism enclosing the target scan region.  
 */

    // Parse Request, Get Parameters
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);
    int SimulationID                 = Util::GetInt(&RequestJSON, "SimulationID");
    float PixelResolution_nm         = Util::GetVec3(&RequestJSON, "PixelResolution_nm", );
    int ImageWidth_px                = Util::GetInt(&RequestJSON, "ImageWidth_px");
    int ImageHeight_px               = Util::GetInt(&RequestJSON, "ImageHeight_px");
    float SliceThickness_nm          = Util::GetFloat(&RequestJSON, "SliceThickness_nm");
    float ScanRegionOverlap_percent  = Util::GetFloat(&RequestJSON, "ScanRegionOverlap_percent");
    Logger_->Log(std::string("VSDA EM SetupMicroscope Called On Simulation With ID ") + std::to_string(SimulationID), 4);

    // Check Sim ID
    if (SimulationID >= Simulations_.size() || SimulationID < 0) { // invlaid id
        Logger_->Log(std::string("VSDA EM SetupMicroscope Error, Simulation With ID ") + std::to_string(SimulationID) + " Does Not Exist", 7);
        nlohmann::json ResponseJSON;
        ResponseJSON["StatusCode"] = 1; // invalid simulation id
        return ResponseJSON.dump();
    }

    Simulation* ThisSimulation = Simulations_[SimulationID].get();
    

    // Build Microscope Paremters Data
    BoundingBox:: Params;
    Params.VoxelResolution_um = PixelResolution_nm*1000; // This is not right, and we'll need to tweak some of these conversions, but for now it's the best we've got.
    Params.ImageWidth_px = ImageWidth_px;
    Params.ImageHeight_px = ImageHeight_px;
    Params.ScanRegionOverlap_percent = ScanRegionOverlap_percent;
    Params.SliceThickness_um = SliceThickness_nm*1000;

    int Status = !VSDADefineScanRegion(Logger_, ThisSimulation, Params);

    // Build Response
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = Status;
    return ResponseJSON.dump();


}
std::string RPCInterface::VSDAEMQueueRenderOperation(std::string _JSONRequest) {

}
std::string RPCInterface::VSDAEMGetRenderStatus(std::string _JSONRequest) {

}
std::string RPCInterface::VSDAEMGetImageStack(std::string _JSONRequest) {

}


}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
