//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EMRenderer.h>

#include <VSDA/VoxelSubsystem/EMSubRegion.h>




namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {


bool ExecuteSubRenderOperations(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Simulation, ImageProcessorPool* _ImageProcessorPool, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool) {

    // Check that the simulation has been initialized and everything is ready to have work done
    if (_Simulation->VSDAData_.State_ != VSDA_RENDER_REQUESTED) {
        return false;
    }
    _Simulation->VSDAData_.State_ = VSDA_RENDER_IN_PROGRESS;
    
    _Logger->Log("Executing Render Job For Requested Simulation", 4);


    // -- Phase 1 --
    // We're going to now generate a list of regions which can then be rendered on this machine
    // to do so, we're going to use a configurable max_voxel_array_dimension to find our max size at this resolution, and then calculate the bounding box based on that
    std::vector<SubRegion> Regions;



    SubRegion ThisRegion;
    ThisRegion.LayerOffset = 0;
    ThisRegion.RegionOffsetX_um = 0.;
    ThisRegion.RegionOffsetY_um = 0.;
    ThisRegion.Sim = _Simulation;
    ThisRegion.Region = _Simulation->VSDAData_.Regions_[_Simulation->VSDAData_.ActiveRegionID_];
    EMRenderSubRegion(_Logger, &ThisRegion, _ImageProcessorPool, _GeneratorPool);    


    _Simulation->VSDAData_.State_ = VSDA_RENDER_DONE;

    return true;

}


// bool ExecuteRenderOperations(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Simulation, ImageProcessorPool* _ImageProcessorPool, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool) {

//     // Check that the simulation has been initialized and everything is ready to have work done
//     if (_Simulation->VSDAData_.State_ != VSDA_RENDER_REQUESTED) {
//         return false;
//     }
//     _Simulation->VSDAData_.State_ = VSDA_RENDER_IN_PROGRESS;
    
//     _Logger->Log("Executing Render Job For Requested Simulation", 4);


//     // Get Requested Region
//     ScanRegion RequestedRegion = _Simulation->VSDAData_.Regions_[_Simulation->VSDAData_.ActiveRegionID_];


//     // Setup Metadata For GetRenderStatus
//     float TotalRegionThickness = abs(RequestedRegion.Point1Z_um - RequestedRegion.Point2Z_um);
//     _Simulation->VSDAData_.TotalSlices_ = TotalRegionThickness / _Simulation->VSDAData_.Params_.VoxelResolution_um;


//     // Create Voxel Array
//     _Logger->Log(std::string("Creating Voxel Array Of Size ") + RequestedRegion.Dimensions() + std::string(" With Points ") + RequestedRegion.ToString(), 2);
//     _Simulation->VSDAData_.Array_ = std::make_unique<VoxelArray>(RequestedRegion, _Simulation->VSDAData_.Params_.VoxelResolution_um);
//     CreateVoxelArrayFromSimulation(_Logger, _Simulation, &_Simulation->VSDAData_.Params_, _Simulation->VSDAData_.Array_.get(), _GeneratorPool);




//     // Clear Scene In Preperation For Rendering
//     _Logger->Log("Starting Slice By Slice Render", 2);
//     // _Renderer->ResetScene();
//     for (unsigned int i = 0; i < _Simulation->VSDAData_.Array_.get()->GetZ(); i++) {
//         std::string FileNamePrefix = "Simulation" + std::to_string(_Simulation->ID) + "_Region" + std::to_string(_Simulation->VSDAData_.ActiveRegionID_);

//         std::vector<std::string> Files = RenderSliceFromArray(_Logger, &_Simulation->VSDAData_, FileNamePrefix, i, _ImageProcessorPool);
//         for (size_t i = 0; i < Files.size(); i++) {
//             _Simulation->VSDAData_.RenderedImagePaths_[_Simulation->VSDAData_.ActiveRegionID_].push_back(Files[i]);
//         }

        
//     }



//     // Ensure All Tasks Are Finished
//     while (_ImageProcessorPool->GetQueueSize() > 0) {

//         // Calculate Desired Image Size
//         // In order for us to deal with multiple different pixel/voxel setting, we create an image of start size where one pixel = 1 voxel
//         // then later on, we resample it to be the right size (for the target image)
//         int VoxelsPerStepX = ceil(_Simulation->VSDAData_.Params_.ImageWidth_px / _Simulation->VSDAData_.Params_.NumPixelsPerVoxel_px);
//         int VoxelsPerStepY = ceil(_Simulation->VSDAData_.Params_.ImageHeight_px / _Simulation->VSDAData_.Params_.NumPixelsPerVoxel_px);
//         int NumChannels = 3;
//         float CameraStepSizeX_um = VoxelsPerStepX * _Simulation->VSDAData_.Params_.VoxelResolution_um;
//         float CameraStepSizeY_um = VoxelsPerStepY * _Simulation->VSDAData_.Params_.VoxelResolution_um;

//         double TotalSliceWidth = abs((double)_Simulation->VSDAData_.Array_->GetBoundingBox().bb_point1[0] - (double)_Simulation->VSDAData_.Array_->GetBoundingBox().bb_point2[0]);
//         double TotalSliceHeight = abs((double)_Simulation->VSDAData_.Array_->GetBoundingBox().bb_point1[1] - (double)_Simulation->VSDAData_.Array_->GetBoundingBox().bb_point2[1]);
//         int TotalXSteps = ceil(TotalSliceWidth / CameraStepSizeX_um);
//         int TotalYSteps = ceil(TotalSliceHeight / CameraStepSizeY_um);

//         int ImagesPerSlice = TotalXSteps * TotalYSteps;


//         // Update Current Slice Information (Account for slice numbers not starting at 0)
//         _Simulation->VSDAData_.TotalSlices_ = _Simulation->VSDAData_.Array_.get()->GetZ();
//         _Simulation->VSDAData_.CurrentSlice_ = _Simulation->VSDAData_.Array_.get()->GetZ() - ceil((float)_ImageProcessorPool->GetQueueSize() / ImagesPerSlice);
//         _Simulation->VSDAData_.TotalSliceImages_ = ImagesPerSlice;
//         _Simulation->VSDAData_.CurrentSliceImage_ = _ImageProcessorPool->GetQueueSize() % ImagesPerSlice;

//         // Log Queue Size
//         _Logger->Log("ImageProcessorPool Queue Length '" + std::to_string(_ImageProcessorPool->GetQueueSize()) + "'", 1);


//         // Now wait a while so we don't spam the console
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));


//     }
//     for (unsigned int i = 0; i < _Simulation->VSDAData_.Tasks_.size(); i++) {
//         ProcessingTask* Task = _Simulation->VSDAData_.Tasks_[i].get();
//         while (Task->IsDone_ != true) {
//             std::this_thread::sleep_for(std::chrono::milliseconds(5));
//         }
//     }

//     _Simulation->VSDAData_.State_ = VSDA_RENDER_DONE;

//     return true;

// }



}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
