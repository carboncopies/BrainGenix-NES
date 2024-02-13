//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/EMSubRegion.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {



bool EMRenderSubRegion(BG::Common::Logger::LoggingSystem* _Logger, SubRegion* _SubRegion, ImageProcessorPool* _ImageProcessorPool, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool) {
    _Logger->Log("Executing SubRegion Render For Region Starting At " + std::to_string(_SubRegion->RegionOffsetX_um) + "X, " + std::to_string(_SubRegion->RegionOffsetY_um) + "Y, Layer " + std::to_string(_SubRegion->LayerOffset), 4);


    // Get Local Variables
    ScanRegion RequestedRegion = _SubRegion->Region;
    Simulation* Sim = _SubRegion->Sim;
    VSDAData* VSDAData_ = &Sim->VSDAData_; 
    int SliceOffset = _SubRegion->LayerOffset;
    double XOffset = _SubRegion->RegionOffsetX_um;
    double YOffset = _SubRegion->RegionOffsetY_um;


    // Setup Metadata For GetRenderStatus
    float TotalRegionThickness = abs(RequestedRegion.Point1Z_um - RequestedRegion.Point2Z_um);
    VSDAData_->TotalSlices_ = TotalRegionThickness / VSDAData_->Params_.VoxelResolution_um;


    // Create Voxel Array
    _Logger->Log(std::string("Creating Voxel Array Of Size ") + RequestedRegion.Dimensions() + std::string(" With Points ") + RequestedRegion.ToString(), 2);
    uint64_t TargetArraySize = RequestedRegion.GetVoxelSize(VSDAData_->Params_.VoxelResolution_um);
    if (VSDAData_->Array_.get() == nullptr || VSDAData_->Array_->GetSize() != TargetArraySize) {
        _Logger->Log("Voxel Array Does Not Exist Yet Or Is Wrong Size, (Re)Creating Now", 2);
        VSDAData_->Array_ = std::make_unique<VoxelArray>(RequestedRegion, VSDAData_->Params_.VoxelResolution_um);
    } else {
        _Logger->Log("Reusing Existing Voxel Array, Clearing Data", 2);
        VSDAData_->Array_->ClearArrayThreaded(std::thread::hardware_concurrency());
        VSDAData_->Array_->SetBB(RequestedRegion);
    }
    CreateVoxelArrayFromSimulation(_Logger, Sim, &VSDAData_->Params_, VSDAData_->Array_.get(), RequestedRegion, _GeneratorPool);



    // Clear Scene In Preperation For Rendering
    for (unsigned int i = 0; i < VSDAData_->Array_.get()->GetZ(); i++) {
        std::string FileNamePrefix = "Simulation" + std::to_string(Sim->ID) + "/Region" + std::to_string(VSDAData_->ActiveRegionID_);

        std::vector<std::string> Files = RenderSliceFromArray(_Logger, _SubRegion->MaxImagesX, _SubRegion->MaxImagesY, &Sim->VSDAData_, VSDAData_->Array_.get(), FileNamePrefix, i, _ImageProcessorPool, XOffset, YOffset, SliceOffset);
        for (size_t i = 0; i < Files.size(); i++) {
            VSDAData_->RenderedImagePaths_[VSDAData_->ActiveRegionID_].push_back(Files[i]);
        }

        
    }



    // Ensure All Tasks Are Finished
    while (_ImageProcessorPool->GetQueueSize() > 0) {

        // Calculate Desired Image Size
        // In order for us to deal with multiple different pixel/voxel setting, we create an image of start size where one pixel = 1 voxel
        // then later on, we resample it to be the right size (for the target image)
        int VoxelsPerStepX = ceil(VSDAData_->Params_.ImageWidth_px / VSDAData_->Params_.NumPixelsPerVoxel_px);
        int VoxelsPerStepY = ceil(VSDAData_->Params_.ImageHeight_px / VSDAData_->Params_.NumPixelsPerVoxel_px);
        int NumChannels = 3;
        float CameraStepSizeX_um = VoxelsPerStepX * VSDAData_->Params_.VoxelResolution_um;
        float CameraStepSizeY_um = VoxelsPerStepY * VSDAData_->Params_.VoxelResolution_um;

        double TotalSliceWidth = abs((double)VSDAData_->Array_->GetBoundingBox().bb_point1[0] - (double)VSDAData_->Array_->GetBoundingBox().bb_point2[0]);
        double TotalSliceHeight = abs((double)VSDAData_->Array_->GetBoundingBox().bb_point1[1] - (double)VSDAData_->Array_->GetBoundingBox().bb_point2[1]);
        int TotalXSteps = ceil(TotalSliceWidth / CameraStepSizeX_um);
        int TotalYSteps = ceil(TotalSliceHeight / CameraStepSizeY_um);

        int ImagesPerSlice = TotalXSteps * TotalYSteps;


        // Update Current Slice Information (Account for slice numbers not starting at 0)
        VSDAData_->TotalSlices_ = VSDAData_->Array_.get()->GetZ();
        VSDAData_->CurrentSlice_ = VSDAData_->Array_.get()->GetZ() - ceil((float)_ImageProcessorPool->GetQueueSize() / ImagesPerSlice);
        VSDAData_->TotalSliceImages_ = ImagesPerSlice;
        VSDAData_->CurrentSliceImage_ = _ImageProcessorPool->GetQueueSize() % ImagesPerSlice;

        // Log Queue Size
        _Logger->Log("ImageProcessorPool Queue Length '" + std::to_string(_ImageProcessorPool->GetQueueSize()) + "'", 1);


        // Now wait a while so we don't spam the console
        std::this_thread::sleep_for(std::chrono::milliseconds(100));


    }
    for (unsigned int i = 0; i < VSDAData_->Tasks_.size(); i++) {
        ProcessingTask* Task = VSDAData_->Tasks_[i].get();
        while (Task->IsDone_ != true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }


    return true;



}




}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
