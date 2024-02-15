//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <filesystem>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/RPCRoutes/Ca.h>
#include <VSDA/Ca/VoxelSubsystem/Structs/CaVoxelArray.h>



namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {

bool VSDA_CA_Initialize(BG::Common::Logger::LoggingSystem* _Logger, Simulator::Simulation* _Sim) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if (_Sim->CaData_.State_ != CA_NOT_INITIALIZED) { // Check if the VSDA has already been initialized, if so - we don't touch it.
        _Logger->Log("VSDA Ca Initialize Error, Cannot Reinit A Region Which Has Already Been Initialized", 6);
        return false; 
    }

    // Initialize it
    _Sim->CaData_.State_ = CA_INIT_BEGIN;


    // That's all we have to do apparently...
    return true;

}

bool VSDA_CA_SetupMicroscope(BG::Common::Logger::LoggingSystem* _Logger, Simulator::Simulation* _Sim, CaMicroscopeParameters _Params) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if ((_Sim->CaData_.State_ != CA_INIT_BEGIN) && (_Sim->CaData_.State_ != CA_RENDER_DONE)) { // Check that the VSDA is during its init phase, and not yet done initializing.
        _Logger->Log("VSDA Ca SetupMicroscope Error, Cannot Setup Microscope On System With Unknown State", 6);
        return false; 
    }

    // Check that Resolution is valid
    if (_Params.VoxelResolution_um == 0) {
        return false;
    }

    // Copy over the parameters
    _Sim->CaData_.Params_ = _Params;

    return true;

}

bool VSDA_CA_DefineScanRegion(BG::Common::Logger::LoggingSystem* _Logger, Simulator::Simulation* _Sim, Simulator::ScanRegion _ScanRegion, int* _RegionID) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if ((_Sim->CaData_.State_ != CA_INIT_BEGIN) && (_Sim->CaData_.State_ != CA_RENDER_DONE)) { // Check that the VSDA is during its init phase, and not yet done initializing.
        _Logger->Log("VSDA Ca DefineScanRegion Error, Cannot Define Microscope Scan Region On System With Unknown State", 6);
        return false; 
    }

    // Copy over the parameters, update region ID
    _Logger->Log(std::string("Adding Scan Region With Dimensions ") + _ScanRegion.Dimensions(), 2);
    _Sim->CaData_.Regions_.push_back(_ScanRegion);
    (*_RegionID) = _Sim->CaData_.Regions_.size()-1;

    // Add New Vector To Store The Rendered Image Paths As We Create Them Later On
    _Sim->CaData_.RenderedImagePaths_.push_back(std::vector<std::string>());

    return true;

}

bool VSDA_CA_QueueRenderOperation(BG::Common::Logger::LoggingSystem* _Logger, Simulator::Simulation* _Sim, int _RegionID) {

    // Check Preconditions
    assert(_Logger != nullptr);
    assert(_Sim != nullptr);
    if ((_Sim->CaData_.State_ != CA_INIT_BEGIN) && (_Sim->CaData_.State_ != CA_RENDER_DONE)) { // Check that the VSDA is during its init phase, and not yet done initializing.
        _Logger->Log("VSDA Ca QueueRenderOperation Error, Cannot Queue Render Operation On System With Unknown State", 6);
        return false; 
    }
    if (_RegionID < 0 || _RegionID >= _Sim->CaData_.Regions_.size()) { // the region ID is out of range
        _Logger->Log("VSDA Ca QueueRenderOperation Error, Region ID Is Out Of Range!", 6);
        return false;    
    }

    if (_Sim->WorkRequested || _Sim->CurrentTask != Simulator::SIMULATION_NONE) { // Cannot start an operation, simulation is busy
        _Logger->Log("VSDA Ca QueueRenderOperation Error, Simulation Is Currently Running!", 6);
        return false;    
    }

    // Setup Enums, Indicate that work is requested
    _Sim->CaData_.ActiveRegionID_ = _RegionID;
    _Sim->CaData_.State_ = CA_RENDER_REQUESTED;
    _Sim->CurrentTask = Simulator::SIMULATION_CALCIUM;
    _Sim->WorkRequested = true;

    return true;

}




}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
