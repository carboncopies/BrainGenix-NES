//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>
#include <algorithm>

#include <unistd.h>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/EMRenderer.h>

#include <VSDA/EM/VoxelSubsystem/EMSubRegion.h>




// https://stackoverflow.com/questions/2513505/how-to-get-available-memory-c-g
unsigned long long getTotalSystemMemory() {
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}



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


    // -- Phase 0 --
    // Here, we detect how much memory this machine has and then use that to make an educated guess as to the max size of the voxel array.
    // This will of course go badly if you're using a lot of ram at the moment, and needs to be improved in the future.

    // Maximum permitted size, we cannot exceed this even if we have more RAM.
    size_t MaxVoxelSizeLimit = 4100; 

    // With this scaling factor, we assume that this is the max portion of the system ram we can use
    // That way, we don't gobble more than say 60% of it in one alloc
    double ScalingFactor = 0.5;


    // Now, calculate the maximum number of voxels in system ram
    uint64_t MaxVoxels = uint64_t(double(getTotalSystemMemory()) * ScalingFactor) / sizeof(VoxelType);
    size_t MaxVoxelArraySizeOnAxisInRAM = std::cbrt(MaxVoxels);

    size_t MaxVoxelArrayAxisSize_vox = std::min(MaxVoxelSizeLimit, MaxVoxelArraySizeOnAxisInRAM);


    // Make Log Message about memory consumption figures
    double MemorySize_MB = ((MaxVoxelArrayAxisSize_vox * MaxVoxelArrayAxisSize_vox * MaxVoxelArrayAxisSize_vox) * sizeof(VoxelType)) / 1024. / 1024;
    double SystemRAM_MB = double(getTotalSystemMemory()) / 1024. / 1024.; 
    std::string LogMessage = "Using Maximum Voxel Array Dimensions Of '" + std::to_string(MaxVoxelArrayAxisSize_vox) + "', This May Use Up To ~" + std::to_string(round(MemorySize_MB)) + "MiB";
    LogMessage += " (" + std::to_string(ScalingFactor*100) + "% of ~" + std::to_string(round(SystemRAM_MB)) + "MiB System Memory)";
    _Logger->Log(LogMessage, 3);



    // -- Phase 1 --
    // We're going to now generate a list of regions which can then be rendered on this machine
    // to do so, we're going to use a configurable max_voxel_array_dimension to find our max size at this resolution, and then calculate the bounding box based on that

    // Calculate Subregion Coordinates that allign with the image step size, that way we don't have any issues where there are gaps in the middle of the image stack.
    // Please note that we're currently assuming images are square, and will probably break if they're not.
    // See the readme.md file in this directory for more information regarding the algorithm and visualizations.

    // Firstly, we calculate the size that each image will be advanced on each subregion.
    // This way, we can calculate the total size of our base region in terms of number of images.
    double ImageStepSizeX_um = (Params->ImageWidth_px / Params->NumPixelsPerVoxel_px) * Params->VoxelResolution_um * (1 - (double(Params->ScanRegionOverlap_percent) / 100.));
    double ImageStepSizeY_um = (Params->ImageHeight_px / Params->NumPixelsPerVoxel_px) * Params->VoxelResolution_um * (1 - (double(Params->ScanRegionOverlap_percent) / 100.));
    int NumImagesInXDimension_img = ceil(BaseRegion->SizeX() / ImageStepSizeX_um);
    int NumImagesInYDimension_img = ceil(BaseRegion->SizeX() / ImageStepSizeY_um);
    _Simulation->VSDAData_.TotalImagesX_ = NumImagesInXDimension_img;
    _Simulation->VSDAData_.TotalImagesY_ = NumImagesInXDimension_img;
    _Logger->Log("Identified The Total Number Of Images To Be '" + std::to_string(NumImagesInXDimension_img) + "'X, '" + std::to_string(NumImagesInYDimension_img) + "'Y", 4);

    // Nextly, we're going to figure out the step size information for the subregions.
    // This will enable us to create subregions that exactly are multiples of the image step sizes, removing any overlap (unless it's on the border, then it may overshoot slightly)
    double MaxVoxelArraySize_um = MaxVoxelArrayAxisSize_vox * Params->VoxelResolution_um;
    int ImagesPerSubRegionX = floor(MaxVoxelArraySize_um / ImageStepSizeX_um);
    int ImagesPerSubRegionY = floor(MaxVoxelArraySize_um / ImageStepSizeY_um);
    if (ImagesPerSubRegionX == 0 || ImagesPerSubRegionY == 0) {
        _Logger->Log("Error, You Don't Have Enough Memory To Render Images At " + std::to_string(Params->ImageWidth_px) + " by " + std::to_string(Params->ImageHeight_px) + " Try Reducing The Image Resolution", 8);
        _Logger->Log("Render Aborted", 9);
        return false;
    }
    double SubRegionStepSizeX_um = ImagesPerSubRegionX * ImageStepSizeX_um;
    double SubRegionStepSizeY_um = ImagesPerSubRegionY * ImageStepSizeY_um;
    double SubRegionStepSizeZ_um = MaxVoxelArrayAxisSize_vox * Params->VoxelResolution_um;
    std::string LogMsg = "Calculated Step Size For Subregions To Be '" + std::to_string(SubRegionStepSizeX_um);
    LogMsg += "'X_um, '" + std::to_string(SubRegionStepSizeY_um) + "'Y_um, '" + std::to_string(SubRegionStepSizeZ_um) + "'Z_um";
    _Logger->Log(LogMsg, 4);

    // Okay, now that we know how big each step for our subregion should be at maximum, we can now calculate the amount they need to overlap in each direction.
    // Note that Z is omitted since we don't overlap slices, just on the x,y axis not z.
    double SubRegionOverlapX_um = (Params->ImageWidth_px / Params->NumPixelsPerVoxel_px) * Params->VoxelResolution_um * (double(Params->ScanRegionOverlap_percent) / 100.);
    double SubRegionOverlapY_um = (Params->ImageHeight_px / Params->NumPixelsPerVoxel_px) * Params->VoxelResolution_um * (double(Params->ScanRegionOverlap_percent) / 100.);


    // Next, we can calculate the number of regions in each dimension
    // Then we will enumerate them later on and calculate the bottom left and top right point for each
    int NumSubRegionsInXDim = ceil(BaseRegion->SizeX() / SubRegionStepSizeX_um);
    int NumSubRegionsInYDim = ceil(BaseRegion->SizeY() / SubRegionStepSizeY_um);
    int NumSubRegionsInZDim = ceil(BaseRegion->SizeZ() / SubRegionStepSizeZ_um);
    _Logger->Log("Calculated '" + std::to_string(NumSubRegionsInXDim) + "'X Subregions", 3);
    _Logger->Log("Calculated '" + std::to_string(NumSubRegionsInYDim) + "'Y Subregions", 3);
    _Logger->Log("Calculated '" + std::to_string(NumSubRegionsInZDim) + "'Z Subregions", 3);


    // Now, we go through all of the steps in each direction that we identified, and calculate the bounding boxes for each
    std::vector<SubRegion> SubRegions;
    for (int XStep = 0; XStep < NumSubRegionsInXDim; XStep++) {
        for (int YStep = 0; YStep < NumSubRegionsInYDim; YStep++) {
            for (int ZStep = 0; ZStep < NumSubRegionsInZDim; ZStep++) {

                // -- Phase 2 --
                // Now, we generate all of the subregions, which is done in this loop.
                // These need to be placed perfectly so that they line up with images from other subregions

                // Now, we need to calculate the offsets for this region
                // This is done since the base region is not guarenteed to be positioned at 0,0,0, it could be placed just about anywhere
                // So, we shift all of our subregions to ensure that they reflect this offset of the base region
                double BaseRegionOffsetX_um = BaseRegion->Point1X_um;
                double BaseRegionOffsetY_um = BaseRegion->Point1Y_um;
                double BaseRegionOffsetZ_um = BaseRegion->Point1Z_um;
                int LayerOffset = ZStep * MaxVoxelArrayAxisSize_vox;

                
                // Next, we're going to calculate the subregion start and ending boxes
                // This is the bottom left coordinate for the subregion
                double SubRegionStartX_um = (XStep * SubRegionStepSizeX_um) + BaseRegionOffsetX_um;
                double SubRegionStartY_um = (YStep * SubRegionStepSizeY_um) + BaseRegionOffsetY_um;
                double SubRegionStartZ_um = (ZStep * SubRegionStepSizeZ_um) + BaseRegionOffsetZ_um;


                // And this is the top right coordinate for the subregion
                // The std::min stuff is to ensure that if we're on the edge of the base region, we don't exceed the user-defined limit
                // In other words, the std::min is to account for the ceil we took earlier with the NumSubRegionsInDim 
                // Also, we add the step of voxelresolution to ensure there are no rounding errors that cause a blue line to occur.
                double SubRegionEndX_um = std::min(BaseRegionOffsetX_um + ((XStep + 1) * SubRegionStepSizeX_um) + SubRegionOverlapX_um + Params->VoxelResolution_um, (double)BaseRegion->Point2X_um);
                double SubRegionEndY_um = std::min(BaseRegionOffsetY_um + ((YStep + 1) * SubRegionStepSizeY_um) + SubRegionOverlapY_um + Params->VoxelResolution_um, (double)BaseRegion->Point2Y_um);
                double SubRegionEndZ_um = std::min(BaseRegionOffsetZ_um + ((ZStep + 1) * SubRegionStepSizeZ_um), (double)BaseRegion->Point2Z_um);



                // Now we can just fill in the structs for this subregion as shown, and append it to the list of subregions to be rendered
                ScanRegion ThisRegion;
                ThisRegion.Point1X_um = SubRegionStartX_um;
                ThisRegion.Point1Y_um = SubRegionStartY_um;
                ThisRegion.Point1Z_um = SubRegionStartZ_um;
                ThisRegion.Point2X_um = SubRegionEndX_um;
                ThisRegion.Point2Y_um = SubRegionEndY_um;
                ThisRegion.Point2Z_um = SubRegionEndZ_um;
                ThisRegion.SampleRotationX_rad = BaseRegion->SampleRotationX_rad;
                ThisRegion.SampleRotationY_rad = BaseRegion->SampleRotationY_rad;
                ThisRegion.SampleRotationZ_rad = BaseRegion->SampleRotationZ_rad;

                SubRegion ThisSubRegion;
                ThisSubRegion.Sim = _Simulation;
                ThisSubRegion.RegionOffsetX_um = SubRegionStartX_um; // We should get rid of these
                ThisSubRegion.RegionOffsetY_um = SubRegionStartY_um; // We should get rid of these
                ThisSubRegion.MaxImagesX = ImagesPerSubRegionX;
                ThisSubRegion.MaxImagesY = ImagesPerSubRegionY;                
                ThisSubRegion.LayerOffset = ZStep * MaxVoxelArrayAxisSize_vox;
                ThisSubRegion.Region = ThisRegion;


                _Logger->Log("Created SubRegion At Location " + ThisRegion.ToString() + " Of Size " + ThisRegion.GetDimensionsInVoxels(Params->VoxelResolution_um), 3);

                SubRegions.push_back(ThisSubRegion);

            }
        }
    }


    _Simulation->VSDAData_.TotalRegions_ = SubRegions.size();


    // -- Phase 3 -- 
    // Now, we're just going to go and render each of the different regions
    // This is done through simply running a for loop, and calling the rendersubregion code on each
    _Logger->Log("Rendering " + std::to_string(SubRegions.size()) + " Sub Regions", 4);
    for (size_t i = 0; i < SubRegions.size(); i++) {
        EMRenderSubRegion(_Logger, &SubRegions[i], _ImageProcessorPool, _GeneratorPool);
        _Simulation->VSDAData_.CurrentRegion_ = i + 1;
    }


    // Now, release memory by creating an empty array, which will cause the unique_ptr for the previous array to be destroyed
    ScanRegion Empty;
    Empty.Point1X_um = 0.;
    Empty.Point1Y_um = 0.;
    Empty.Point1Z_um = 0.;
    Empty.Point2X_um = 0.;
    Empty.Point2Y_um = 0.;
    Empty.Point2Z_um = 0.;
    _Simulation->VSDAData_.Array_ = std::make_unique<VoxelArray>(_Logger, Empty, 999.);
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
