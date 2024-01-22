//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <filesystem>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/VoxelSubsystem/VoxelArrayRenderer.h>

#include <VSDA/VoxelSubsystem/ImageProcessorPool/Image.h>




namespace BG {
namespace NES {
namespace Simulator {

// TODO: Make renderslicefromarray use the imageprocessorpool to do the enumeration of the voxel array (the pixel population part) using threads (so that nested for loop goes away, and this therad just positions the cameras)
// Then, if needed, we can move renderslicefromarray inside the threadedrendervoxelarray if that still isn't fast enough.


// std::vector<std::vector<std::string>> ThreadedRenderVoxelArray(BG::Common::Logger::LoggingSystem* _Logger, VSDAData* _VSDAData, std::string _FilePrefix, ImageProcessorPool* _ImageProcessorPool) {

// }


std::vector<std::string> RenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, VSDAData* _VSDAData, std::string _FilePrefix, int SliceNumber, ImageProcessorPool* _ImageProcessorPool) {
    assert(_VSDAData != nullptr);
    assert(_Logger != nullptr);


    // Get Params and Array From VSDAData
    MicroscopeParameters* Params = &_VSDAData->Params_;
    VoxelArray* Array = _VSDAData->Array_.get();
    float VoxelSize = Array->GetResolution();
    BoundingBox VoxelBB = Array->GetBoundingBox();

    // _Logger->Log(std::string("Rendering Slice '") + std::to_string(SliceNumber) + "'", 1);


    std::vector<std::string> Filenames;

    
    // Calculate Desired Image Size
    // In order for us to deal with multiple different pixel/voxel setting, we create an image of start size where one pixel = 1 voxel
    // then later on, we resample it to be the right size (for the target image)
    int VoxelsPerStepX = ceil(_VSDAData->Params_.ImageWidth_px / _VSDAData->Params_.NumPixelsPerVoxel_px);
    int VoxelsPerStepY = ceil(_VSDAData->Params_.ImageHeight_px / _VSDAData->Params_.NumPixelsPerVoxel_px);
    int NumChannels = 3;
    float CameraStepSizeX_um = VoxelsPerStepX * _VSDAData->Params_.VoxelResolution_um;
    float CameraStepSizeY_um = VoxelsPerStepY * _VSDAData->Params_.VoxelResolution_um;

    double TotalSliceWidth = abs((double)Array->GetBoundingBox().bb_point1[0] - (double)Array->GetBoundingBox().bb_point2[0]);
    double TotalSliceHeight = abs((double)Array->GetBoundingBox().bb_point1[1] - (double)Array->GetBoundingBox().bb_point2[1]);
    int TotalXSteps = ceil(TotalSliceWidth / CameraStepSizeX_um);
    int TotalYSteps = ceil(TotalSliceHeight / CameraStepSizeY_um);


    // Update Stats With Total Number Of Images
    // _VSDAData->TotalSliceImages_ = TotalXSteps * TotalYSteps;
    // _Logger->Log("Image Stack Will Have " + std::to_string(TotalXSteps) + " X Steps, " + std::to_string(TotalYSteps) + " Y Steps", 4);


    // Now, we enumerate through all the steps needed, one at a time until we reach the end
    for (int XStep = 0; XStep < TotalXSteps; XStep++) {
        for (int YStep = 0; YStep < TotalYSteps; YStep++) {

            // Calculate the filename of the image to be generated, add to list of generated images
            std::string FilePath = "Renders/" + _FilePrefix + "_Slice" + std::to_string(SliceNumber);
            FilePath += "_X" + std::to_string(CameraStepSizeX_um * XStep) + "_Y" + std::to_string(CameraStepSizeY_um * YStep) + ".png";

            Filenames.push_back(FilePath);


            // Setup and submit task to queue for rendering
            std::unique_ptr<ProcessingTask> ThisTask = std::make_unique<ProcessingTask>();
            ThisTask->Array_ = _VSDAData->Array_.get();
            ThisTask->Width_px = _VSDAData->Params_.ImageWidth_px;
            ThisTask->Height_px = _VSDAData->Params_.ImageHeight_px;
            ThisTask->VoxelStartingX = VoxelsPerStepX * XStep;
            ThisTask->VoxelStartingY = VoxelsPerStepY * YStep;
            ThisTask->VoxelEndingX = ThisTask->VoxelStartingX + VoxelsPerStepX;
            ThisTask->VoxelEndingY = ThisTask->VoxelStartingY + VoxelsPerStepY;
            ThisTask->VoxelZ = SliceNumber;
            ThisTask->TargetFileName_ = FilePath;

            _ImageProcessorPool->QueueEncodeOperation(ThisTask.get());
            _VSDAData->Tasks_.push_back(std::move(ThisTask));




        }
    }


    return Filenames;
}





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
