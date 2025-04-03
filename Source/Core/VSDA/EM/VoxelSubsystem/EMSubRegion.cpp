//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/EMSubRegion.h>

#include <VSDA/EM/MeshGenerator/MeshingStage.h>



namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {



// Returns:
//   true upon success.
//   false upon failure, and set the std::error_code & err accordingly.
// https://stackoverflow.com/questions/71658440/c17-create-directories-automatically-given-a-file-path
bool VSCreateDirectoryRecursive3(std::string const & dirName, std::error_code & err) {
    err.clear();
    if (!std::filesystem::create_directories(dirName, err))
    {
        if (std::filesystem::exists(dirName))
        {
            // The folder already exists:
            err.clear();
            return true;    
        }
        return false;
    }
    return true;
}


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

    
    // Force us to wait for any other renders using the image processor pool
    while (_ImageProcessorPool->GetQueueSize() > 0) {

        // Update Current Slice Information (Account for slice numbers not starting at 0)
        VSDAData_->CurrentOperation_ = "Image Processing Enqueued";
        VSDAData_->TotalSliceImages_ = 0;
        VSDAData_->CurrentSliceImage_ = 0;
        VSDAData_->VoxelQueueLength_ = 0;
        VSDAData_->TotalVoxelQueueLength_ = 0;
        VSDAData_->TotalSlices_ = 0;
        VSDAData_->CurrentSlice_ = 0;
        VSDAData_->CurrentSlice_ = VSDAData_->TotalSlices_ - _ImageProcessorPool->GetQueueSize();

        _Logger->Log("Waiting for ImageProcessorPool to become available; '" + std::to_string(_ImageProcessorPool->GetQueueSize()) + "' items remaining", 1);

        // Now wait a while so we don't spam the console
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    }

    
    // Update Status Bar
    VSDAData_->CurrentOperation_ = "Image Processing";
    VSDAData_->TotalSliceImages_ = 0;
    VSDAData_->CurrentSliceImage_ = 0;
    VSDAData_->VoxelQueueLength_ = 0;
    VSDAData_->TotalVoxelQueueLength_ = 0;
    VSDAData_->TotalSlices_ = 0;
    VSDAData_->CurrentSlice_ = 0;


    // Clear Scene In Preperation For Rendering
    VSDAData_->Params_.SegmentationResolutionX_px_ = VSDAData_->Array_->GetX();
    VSDAData_->Params_.SegmentationResolutionY_px_ = VSDAData_->Array_->GetY();
    VSDAData_->Params_.SegmentationResolutionZ_px_ = NumVoxelsPerSlice * SEGMENTATION_BLOCK_SIZE;


    noise::module::Perlin PerlinGenerator;
    for (int i = 0; i < NumZSlices; i++) {
        int CurrentSliceIndex = i * NumVoxelsPerSlice;
        std::string FileNamePrefix = "Simulation" + std::to_string(Sim->ID) + "/Region" + std::to_string(VSDAData_->ActiveRegionID_);

        VSDAData_->TotalSlices_ += RenderSliceFromArray(_Logger, _SubRegion->MaxImagesX, _SubRegion->MaxImagesY, &Sim->VSDAData_, VSDAData_->Array_.get(), FileNamePrefix, CurrentSliceIndex, NumVoxelsPerSlice, _ImageProcessorPool, XOffset, YOffset, _SubRegion->MasterRegionOffsetX_um, _SubRegion->MasterRegionOffsetY_um, SliceOffset, &PerlinGenerator);


    }



    // Ensure All Tasks Are Finished
    while (_ImageProcessorPool->GetQueueSize() > 0) {

        // Update Current Slice Information (Account for slice numbers not starting at 0)
        VSDAData_->CurrentSlice_ = VSDAData_->TotalSlices_ - _ImageProcessorPool->GetQueueSize();

        _Logger->Log("ImageProcessorPool Queue Length '" + std::to_string(_ImageProcessorPool->GetQueueSize()) + "'", 1);

        // Now wait a while so we don't spam the console
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));


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
