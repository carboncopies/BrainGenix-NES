//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <filesystem>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/RPCRoutes/EM.h>
#include <Simulator/Structs/Simulation.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>

namespace BG {
namespace NES {
namespace Simulator {


bool VSDA_EM_Initialize(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if (_Sim->VSDAData_.State_ != VSDA_NOT_INITIALIZED) { // Check if the VSDA has already been initialized, if so - we don't touch it.
        _Logger->Log("VSDA EM Initialize Error, Cannot Reinit A Region Which Has Already Been Initialized", 6);
        return false; 
    }

    // Initialize it
    _Sim->VSDAData_.State_ = VSDA_INIT_BEGIN;


    // That's all we have to do apparently...
    return true;

}

bool VSDA_EM_SetupMicroscope(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters _Params) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if ((_Sim->VSDAData_.State_ != VSDA_INIT_BEGIN) && (_Sim->VSDAData_.State_ != VSDA_RENDER_DONE)) { // Check that the VSDA is during its init phase, and not yet done initializing.
        _Logger->Log("VSDA EM SetupMicroscope Error, Cannot Setup Microscope On System With Unknown State", 6);
        return false; 
    }

    // Check that Resolution is valid
    if (_Params.VoxelResolution_um == 0) {
        return false;
    }

    // Copy over the parameters
    _Sim->VSDAData_.Params_ = _Params;

    return true;

}

bool VSDA_EM_DefineScanRegion(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, ScanRegion _ScanRegion, int* _RegionID) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if ((_Sim->VSDAData_.State_ != VSDA_INIT_BEGIN) && (_Sim->VSDAData_.State_ != VSDA_RENDER_DONE)) { // Check that the VSDA is during its init phase, and not yet done initializing.
        _Logger->Log("VSDA EM DefineScanRegion Error, Cannot Define Microscope Scan Region On System With Unknown State", 6);
        return false; 
    }

    // Copy over the parameters, update region ID
    _Logger->Log(std::string("Adding Scan Region With Dimensions ") + _ScanRegion.Dimensions(), 2);
    _Sim->VSDAData_.Regions_.push_back(_ScanRegion);
    (*_RegionID) = _Sim->VSDAData_.Regions_.size()-1;

    // Add New Vector To Store The Rendered Image Paths As We Create Them Later On
    _Sim->VSDAData_.RenderedImagePaths_.push_back(std::vector<std::string>());

    return true;

}

bool VSDA_EM_QueueRenderOperation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, int _RegionID) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if ((_Sim->VSDAData_.State_ != VSDA_INIT_BEGIN) && (_Sim->VSDAData_.State_ != VSDA_RENDER_DONE)) { // Check that the VSDA is during its init phase, and not yet done initializing.
        _Logger->Log("VSDA EM QueueRenderOperation Error, Cannot Queue Render Operation On System With Unknown State", 6);
        return false; 
    }
    if (_RegionID < 0 || _RegionID >= _Sim->VSDAData_.Regions_.size()) { // the region ID is out of range
        _Logger->Log("VSDA EM QueueRenderOperation Error, Region ID Is Out Of Range!", 6);
        return false;    
    }

    if (_Sim->WorkRequested || _Sim->CurrentTask != SIMULATION_NONE) { // Cannot start an operation, simulation is busy
        _Logger->Log("VSDA EM QueueRenderOperation Error, Simulation Is Currently Running!", 6);
        return false;    
    }

    // Setup Enums, Indicate that work is requested
    _Sim->VSDAData_.ActiveRegionID_ = _RegionID;
    _Sim->VSDAData_.State_ = VSDA_RENDER_REQUESTED;
    _Sim->CurrentTask = SIMULATION_VSDA;
    _Sim->WorkRequested = true;

    return true;

}





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
