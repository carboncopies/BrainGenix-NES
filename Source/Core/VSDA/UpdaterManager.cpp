#include <VSDA/UpdaterManager.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {

void CreateDiagonalVoxelLine(VoxelArray* _Arr) {
    for (unsigned int i = 0; i < _Arr->GetZ(); i++) {
        _Arr->SetVoxel(i, i, i, BORDER);
    }
}

bool ExecuteRenderOperations(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Simulation, BG::NES::Renderer::Interface* _Renderer) {

    // Check that the simulation has been initialized and everything is ready to have work done
    if (_Simulation->VSDAData_.State_ != VSDA_RENDER_REQUESTED) {
        return false;
    }

    _Logger->Log("Executing Render Job For Requested Simulation", 4);



    // Create Voxel Array
    ScanRegion RequestedRegion = _Simulation->VSDAData_.Regions_[_Simulation->VSDAData_.ActiveRegionID_];
    _Logger->Log(std::string("Creating Voxel Array Of Size ") + RequestedRegion.Dimensions() + std::string(" With Points ") + RequestedRegion.ToString(), 2);
    _Simulation->VSDAData_.Array_ = std::make_unique<VoxelArray>(RequestedRegion, _Simulation->VSDAData_.Params_.VoxelResolution_um);
    CreateVoxelArrayFromSimulation(_Logger, _Simulation, &_Simulation->VSDAData_.Params_, _Simulation->VSDAData_.Array_.get());


    // CreateDiagonalVoxelLine(_Simulation->VSDAData_.Array_.get());


    // Clear Scene In Preperation For Rendering
    _Logger->Log("Starting Slice By Slice Render", 2);
    _Renderer->ResetScene();
    for (unsigned int i = 0; i < _Simulation->VSDAData_.Array_.get()->GetZ(); i++) {
        std::string FileNamePrefix = "Simulation" + std::to_string(_Simulation->ID) + "_Region" + std::to_string(_Simulation->VSDAData_.ActiveRegionID_);
        std::vector<std::string>* RegionSpecificFilePathList = &_Simulation->VSDAData_.RenderedImagePaths_[_Simulation->VSDAData_.ActiveRegionID_];
        RenderSliceFromArray(_Logger, _Renderer, &_Simulation->VSDAData_.Params_, _Simulation->VSDAData_.Array_.get(), RegionSpecificFilePathList, FileNamePrefix, i);
        // std::this_thread::sleep_for(std::chrono::milliseconds(100)); // delay so our mutex is easier to get by another thread if needed
    }

    return true;

}



}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
