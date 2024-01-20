//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EMRenderer.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {


bool ExecuteRenderOperations(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Simulation, ImageProcessorPool* _ImageProcessorPool, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool) {

    // Check that the simulation has been initialized and everything is ready to have work done
    if (_Simulation->VSDAData_.State_ != VSDA_RENDER_REQUESTED) {
        return false;
    }
    _Simulation->VSDAData_.State_ = VSDA_RENDER_IN_PROGRESS;
    
    _Logger->Log("Executing Render Job For Requested Simulation", 4);


    // Get Requested Region
    ScanRegion RequestedRegion = _Simulation->VSDAData_.Regions_[_Simulation->VSDAData_.ActiveRegionID_];


    // Setup Metadata For GetRenderStatus
    float TotalRegionThickness = abs(RequestedRegion.Point1Z_um - RequestedRegion.Point2Z_um);
    _Simulation->VSDAData_.TotalSlices_ = TotalRegionThickness / _Simulation->VSDAData_.Params_.VoxelResolution_um;


    // Create Voxel Array
    _Logger->Log(std::string("Creating Voxel Array Of Size ") + RequestedRegion.Dimensions() + std::string(" With Points ") + RequestedRegion.ToString(), 2);
    _Simulation->VSDAData_.Array_ = std::make_unique<VoxelArray>(RequestedRegion, _Simulation->VSDAData_.Params_.VoxelResolution_um);
    CreateVoxelArrayFromSimulation(_Logger, _Simulation, &_Simulation->VSDAData_.Params_, _Simulation->VSDAData_.Array_.get(), _GeneratorPool);


    // Set Image Size
    // _Renderer->SetResolution(_Simulation->VSDAData_.Params_.ImageWidth_px, _Simulation->VSDAData_.Params_.ImageHeight_px);
    // _Renderer->DrawFrame();



    // Clear Scene In Preperation For Rendering
    _Logger->Log("Starting Slice By Slice Render", 2);
    // _Renderer->ResetScene();
    for (unsigned int i = 0; i < _Simulation->VSDAData_.Array_.get()->GetZ(); i++) {
        std::string FileNamePrefix = "Simulation" + std::to_string(_Simulation->ID) + "_Region" + std::to_string(_Simulation->VSDAData_.ActiveRegionID_);

        std::vector<std::string> Files = RenderSliceFromArray(_Logger, &_Simulation->VSDAData_, FileNamePrefix, i, _ImageProcessorPool);
        for (size_t i = 0; i < Files.size(); i++) {
            _Simulation->VSDAData_.RenderedImagePaths_[_Simulation->VSDAData_.ActiveRegionID_].push_back(Files[i]);
        }

        // Update Current Slice Information (Account for slice numbers not starting at 0)
        _Simulation->VSDAData_.TotalSlices_ = _Simulation->VSDAData_.Array_.get()->GetZ();
        _Simulation->VSDAData_.CurrentSlice_ = i + 1;
    }



    // Ensure All Tasks Are Finished
    for (unsigned int i = 0; i < _Simulation->VSDAData_.Tasks_.size(); i++) {
        ProcessingTask* Task = _Simulation->VSDAData_.Tasks_[i].get();
        while (Task->IsDone_ != true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    _Simulation->VSDAData_.State_ = VSDA_RENDER_DONE;

    return true;

}



}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
