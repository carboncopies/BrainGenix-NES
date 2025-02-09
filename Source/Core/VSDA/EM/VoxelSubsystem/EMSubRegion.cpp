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


    // todo: fix array claring not working for some reason? (also move back create voxel array from simulation call to other place)

    // Update Status
    VSDAData_->CurrentOperation_ = "Allocating Voxel Array";
    VSDAData_->TotalSliceImages_ = 0;
    VSDAData_->CurrentSliceImage_ = 0;
    VSDAData_->VoxelQueueLength_ = 0;
    VSDAData_->TotalVoxelQueueLength_ = 0;

    // Create Voxel Array
    _Logger->Log(std::string("Creating Voxel Array Of Size ") + RequestedRegion.Dimensions() + std::string(" With Points ") + RequestedRegion.ToString(), 2);
    uint64_t TargetArraySize = RequestedRegion.GetVoxelSize(VSDAData_->Params_.VoxelResolution_um);
    if (VSDAData_->Array_.get() == nullptr || VSDAData_->Array_->GetSize() <= TargetArraySize) {
        _Logger->Log("Voxel Array Does Not Exist Yet Or Is Wrong Size, (Re)Creating Now", 2);
        VSDAData_->Array_ = std::make_unique<VoxelArray>(_Logger, ScanRegion(), 99.);
        VSDAData_->Array_ = std::make_unique<VoxelArray>(_Logger, RequestedRegion, VSDAData_->Params_.VoxelResolution_um);
    } else {
        _Logger->Log("Reusing Existing Voxel Array, Clearing Data", 2);
        bool Status = VSDAData_->Array_->SetSize(RequestedRegion, VSDAData_->Params_.VoxelResolution_um);
        if (!Status) {
            _Logger->Log("Critical Internal Error, Failed to Set Size Of Voxel Array! This Should NEVER HAPPEN", 10);
            exit(999);
        }
        VSDAData_->Array_->ClearArrayThreaded(std::thread::hardware_concurrency());
        // VSDAData_->Array_->ClearArray();
        VSDAData_->Array_->SetBB(RequestedRegion);
    }


    // Initialize Stats
    VSDAData_->TotalSlices_ = 0;
    VSDAData_->CurrentSlice_ = 0;
    VSDAData_->CurrentOperation_ = "Rasterization Preprocessing";
    VSDAData_->TotalSliceImages_ = 0;
    VSDAData_->CurrentSliceImage_ = 0;
    VSDAData_->VoxelQueueLength_ = 0;
    VSDAData_->TotalVoxelQueueLength_ = 0;

    CreateVoxelArrayFromSimulation(_Logger, Sim, &VSDAData_->Params_, VSDAData_->Array_.get(), RequestedRegion, _GeneratorPool);



    // Calculate Number Of Steps For The Z Value
    int NumVoxelsPerSlice = VSDAData_->Params_.SliceThickness_um / VSDAData_->Params_.VoxelResolution_um;
    if (NumVoxelsPerSlice < 1) {
        NumVoxelsPerSlice = 1; // Ensure that there is at least one voxel per slice, otherwise the numzslices calc breaks
    }
    int NumZSlices = ceil((float)VSDAData_->Array_.get()->GetZ() / (float)NumVoxelsPerSlice);

    _Logger->Log("This EM Render Operation Desires " + std::to_string(VSDAData_->Params_.SliceThickness_um) + "um Slice Thickness", 5);
    _Logger->Log("Therefore, we are using " + std::to_string(NumVoxelsPerSlice) + "vox per slice at " + std::to_string(VSDAData_->Params_.VoxelResolution_um) + "um per vox", 5);
    _Logger->Log("We Will Render A Total Of " + std::to_string(NumZSlices) + " Slices", 5);
    

    // Update Status Bar
    VSDAData_->CurrentOperation_ = "Image Processing";
    VSDAData_->TotalSliceImages_ = 0;
    VSDAData_->CurrentSliceImage_ = 0;
    VSDAData_->VoxelQueueLength_ = 0;
    VSDAData_->TotalVoxelQueueLength_ = 0;
    VSDAData_->TotalSlices_ = 0;
    VSDAData_->CurrentSlice_ = 0;


    // Clear Scene In Preperation For Rendering
    noise::module::Perlin PerlinGenerator;
    for (int i = 0; i < NumZSlices; i++) {
        int CurrentSliceIndex = i * NumVoxelsPerSlice;
        std::string FileNamePrefix = "Simulation" + std::to_string(Sim->ID) + "/Region" + std::to_string(VSDAData_->ActiveRegionID_);

        VSDAData_->TotalSlices_ += RenderSliceFromArray(_Logger, _SubRegion->MaxImagesX, _SubRegion->MaxImagesY, &Sim->VSDAData_, VSDAData_->Array_.get(), FileNamePrefix, CurrentSliceIndex, NumVoxelsPerSlice, _ImageProcessorPool, XOffset, YOffset, _SubRegion->MasterRegionOffsetX_um, _SubRegion->MasterRegionOffsetY_um, SliceOffset, &PerlinGenerator);
        // for (size_t x = 0; x < Files.size(); x++) {
        //     VSDAData_->RenderedImagePaths_[VSDAData_->ActiveRegionID_].push_back(Files[x]);
        // }

        // Now generate segmentation map data

        // Calculate the filename of the image to be generated, add to list of generated images
        int AdjustedSliceNumber = (CurrentSliceIndex + SliceOffset) / (VSDAData_->Params_.SliceThickness_um / VSDAData_->Params_.VoxelResolution_um);
        std::string DirectoryPath = "Renders/" + FileNamePrefix + "/Slice" + std::to_string(AdjustedSliceNumber);

        std::unique_ptr<SegmentationCompressionTask> SegTask = std::make_unique<SegmentationCompressionTask>();
        SegTask->Voxels_ = VSDAData_->Array_.get();
        SegTask->ZLevel_ = CurrentSliceIndex;
        SegTask->OutputPath_ = DirectoryPath;
        // SegTask->Params_ = &_VSDAData->Params_;

        // Enqueue Work Operation
        _ImageProcessorPool->QueueEncodeOperation(SegTask.get());
        VSDAData_->Tasks_.push_back(std::move(SegTask));


        VoxelIndexInfo Info;
        Info.StartX = 0;
        Info.EndX = VSDAData_->Array_->GetX();
        Info.StartY = 0;
        Info.EndY = VSDAData_->Array_->GetY();
        Info.StartZ = AdjustedSliceNumber;
        Info.EndZ = AdjustedSliceNumber + 8;

        VSDAData_->Regions_[VSDAData_->ActiveRegionID_].SegmentationFilenames_.push_back(DirectoryPath + "Segmentation");
        VSDAData_->Regions_[VSDAData_->ActiveRegionID_].SegmentationVoxelIndexes_.push_back(Info);



    }



    // Ensure All Tasks Are Finished
    while (_ImageProcessorPool->GetQueueSize() > 0) {

        // // Calculate Desired Image Size
        // // In order for us to deal with multiple different pixel/voxel setting, we create an image of start size where one pixel = 1 voxel
        // // then later on, we resample it to be the right size (for the target image)
        // int VoxelsPerStepX = ceil(VSDAData_->Params_.ImageWidth_px / VSDAData_->Params_.NumPixelsPerVoxel_px);
        // int VoxelsPerStepY = ceil(VSDAData_->Params_.ImageHeight_px / VSDAData_->Params_.NumPixelsPerVoxel_px);
        // int NumChannels = 3;
        // float CameraStepSizeX_um = VoxelsPerStepX * VSDAData_->Params_.VoxelResolution_um;
        // float CameraStepSizeY_um = VoxelsPerStepY * VSDAData_->Params_.VoxelResolution_um;

        // double TotalSliceWidth = abs((double)VSDAData_->Array_->GetBoundingBox().bb_point1[0] - (double)VSDAData_->Array_->GetBoundingBox().bb_point2[0]);
        // double TotalSliceHeight = abs((double)VSDAData_->Array_->GetBoundingBox().bb_point1[1] - (double)VSDAData_->Array_->GetBoundingBox().bb_point2[1]);
        // int TotalXSteps = ceil(TotalSliceWidth / CameraStepSizeX_um);
        // int TotalYSteps = ceil(TotalSliceHeight / CameraStepSizeY_um);
        // TotalXSteps = std::min(TotalXSteps, _SubRegion->MaxImagesX);
        // TotalYSteps = std::min(TotalYSteps, _SubRegion->MaxImagesY);

        // int ImagesPerSlice = TotalXSteps * TotalYSteps;


        // Update Current Slice Information (Account for slice numbers not starting at 0)
        VSDAData_->CurrentSlice_ = VSDAData_->TotalSlices_ - _ImageProcessorPool->GetQueueSize();
        // VSDAData_->TotalSliceImages_ = ImagesPerSlice;
        // VSDAData_->CurrentSliceImage_ = _ImageProcessorPool->GetQueueSize() % ImagesPerSlice;

        // Log Queue Size
        _Logger->Log("ImageProcessorPool Queue Length '" + std::to_string(_ImageProcessorPool->GetQueueSize()) + "'", 1);


        // Now wait a while so we don't spam the console
        std::this_thread::sleep_for(std::chrono::milliseconds(500));


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
