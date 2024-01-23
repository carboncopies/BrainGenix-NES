//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>
#include <algorithm>

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


    // Unpack Variables For Easier Access
    MicroscopeParameters* Params = &_Simulation->VSDAData_.Params_;
    ScanRegion* BaseRegion = &_Simulation->VSDAData_.Regions_[_Simulation->VSDAData_.ActiveRegionID_];


    // -- Phase 1 --
    // We're going to now generate a list of regions which can then be rendered on this machine
    // to do so, we're going to use a configurable max_voxel_array_dimension to find our max size at this resolution, and then calculate the bounding box based on that
    size_t MaxVoxelArrayAxisSize = 1000; // Hard-coded for now, will eventually do this based on hardware limitations

    // Now, we're going to calculate the max size of the voxel array in microns on axis given this max voxel count and our VSDAData voxel size parameter
    double SubRegionStepSize_um = (double)MaxVoxelArrayAxisSize * Params->VoxelResolution_um;
    
    // Next, we identify the number of regions needed in each direction
    int NumXSteps = ceil(BaseRegion->SizeX() / SubRegionStepSize_um);
    int NumYSteps = ceil(BaseRegion->SizeY() / SubRegionStepSize_um);
    int NumZSteps = ceil(BaseRegion->SizeZ() / SubRegionStepSize_um);

    // Finally, we create the SubRegion array and fill it up with this info
    std::vector<SubRegion> SubRegions;
    for (int XStep = 0; XStep < NumXSteps; XStep++) {
        for (int YStep = 0; YStep < NumYSteps; YStep++) {
            for (int ZStep = 0; ZStep < NumZSteps; ZStep++) {

                // Calculate offsets/points for this region
                double OffsetX_um = XStep * SubRegionStepSize_um;
                double OffsetY_um = YStep * SubRegionStepSize_um;
                double OffsetZ_um = ZStep * SubRegionStepSize_um;
                int LayerOffset = ZStep * MaxVoxelArrayAxisSize;

                // Now we fill in the region with the parameters for this part
                ScanRegion ThisRegion;
                ThisRegion.Point1X_um = OffsetX_um + BaseRegion->Point1X_um;
                ThisRegion.Point1Y_um = OffsetY_um + BaseRegion->Point1Y_um;
                ThisRegion.Point1Z_um = OffsetZ_um + BaseRegion->Point1Z_um;
                ThisRegion.Point2X_um = std::min(OffsetX_um + SubRegionStepSize_um, (double)BaseRegion->Point2X_um);
                ThisRegion.Point2Y_um = std::min(OffsetY_um + SubRegionStepSize_um, (double)BaseRegion->Point2Y_um);
                ThisRegion.Point2Z_um = std::min(OffsetZ_um + SubRegionStepSize_um, (double)BaseRegion->Point2Z_um);

                SubRegion ThisSubRegion;
                ThisSubRegion.Sim = _Simulation;
                ThisSubRegion.RegionOffsetX_um = XStep * SubRegionStepSize_um;
                ThisSubRegion.RegionOffsetY_um = YStep * SubRegionStepSize_um;
                ThisSubRegion.LayerOffset = ZStep * MaxVoxelArrayAxisSize;
                ThisSubRegion.Region = ThisRegion;


                _Logger->Log("Created SubRegion At Location " + ThisRegion.ToString(), 3);

                SubRegions.push_back(ThisSubRegion);

            }
        }
    }


    // -- Phase 2 -- 
    // Now, we're just going to go and render each of the different regions
    // This is done through simply running a for loop, and calling the rendersubregion code on each
    _Logger->Log("Rendering " + std::to_string(SubRegions.size()) + " Sub Regions", 4);
    for (size_t i = 0; i < SubRegions.size(); i++) {
        EMRenderSubRegion(_Logger, &SubRegions[i], _ImageProcessorPool, _GeneratorPool);
    }



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
