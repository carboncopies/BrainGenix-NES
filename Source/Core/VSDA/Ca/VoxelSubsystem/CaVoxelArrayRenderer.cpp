//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <filesystem>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/Ca/VoxelSubsystem/CaVoxelArrayRenderer.h>

#include <VSDA/Ca/VoxelSubsystem/ImageProcessorPool/Image.h>




namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {


std::vector<std::string> CaRenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, int MaxImagesX, int MaxImagesY, CalciumImagingData* _CaData, VoxelArray* _Array, std::string _FilePrefix, int SliceNumber, ImageProcessorPool* _ImageProcessorPool, double _OffsetX, double _OffsetY, int _SliceOffset) {
    assert(_CaData != nullptr);
    assert(_Logger != nullptr);


    // Get Params and Array From VSDAData
    CaMicroscopeParameters* Params = &_CaData->Params_;
    VoxelArray* Array = _Array;
    float VoxelSize = Array->GetResolution();
    Simulator::BoundingBox VoxelBB = Array->GetBoundingBox();

    // _Logger->Log(std::string("Rendering Slice '") + std::to_string(SliceNumber) + "'", 1);


    std::vector<std::string> Filenames;

    
    // Calculate Desired Image Size
    // In order for us to deal with multiple different pixel/voxel setting, we create an image of start size where one pixel = 1 voxel
    // then later on, we resample it to be the right size (for the target image)
    // The number of voxels we move per step depends on image size and overlap percentage:
    int ImageWidth_vox = ceil(_CaData->Params_.ImageWidth_px / _CaData->Params_.NumPixelsPerVoxel_px);
    int ImageHeight_vox = ceil(_CaData->Params_.ImageHeight_px / _CaData->Params_.NumPixelsPerVoxel_px);
    int VoxelsPerStepX = ceil(_CaData->Params_.ImageWidth_px * (1. - (_CaData->Params_.ScanRegionOverlap_percent / 100.)) / _CaData->Params_.NumPixelsPerVoxel_px);
    int VoxelsPerStepY = ceil(_CaData->Params_.ImageHeight_px * (1. - (_CaData->Params_.ScanRegionOverlap_percent / 100.)) / _CaData->Params_.NumPixelsPerVoxel_px);
    int VoxelsOverlapX = ceil(_CaData->Params_.ImageWidth_px * (_CaData->Params_.ScanRegionOverlap_percent / 100.) / _CaData->Params_.NumPixelsPerVoxel_px);
    int VoxelsOverlapY = ceil(_CaData->Params_.ImageHeight_px * (_CaData->Params_.ScanRegionOverlap_percent / 100.) / _CaData->Params_.NumPixelsPerVoxel_px);
    int NumChannels = 3;
    float CameraStepSizeX_um = VoxelsPerStepX * _CaData->Params_.VoxelResolution_um;
    float CameraStepSizeY_um = VoxelsPerStepY * _CaData->Params_.VoxelResolution_um;

    double TotalSliceWidth = abs((double)Array->GetBoundingBox().bb_point1[0] - (double)Array->GetBoundingBox().bb_point2[0]);
    double TotalSliceHeight = abs((double)Array->GetBoundingBox().bb_point1[1] - (double)Array->GetBoundingBox().bb_point2[1]);

    // Number of X*Y images to take to cover the whole slice:
    int TotalXSteps = ceil(TotalSliceWidth / CameraStepSizeX_um);
    int TotalYSteps = ceil(TotalSliceHeight / CameraStepSizeY_um);

    // Limit the total steps to those defined in the task
    TotalXSteps = std::min(TotalXSteps, MaxImagesX);
    TotalYSteps = std::min(TotalYSteps, MaxImagesY);


    // Make sure we render this slice at all timesteps
    if (_CaData->CalciumConcentrationByIndex_->size() <= 0) {
        _Logger->Log("Invalid Number Of Calcium Concentratiosn For Compartments, Aborting Render", 6);
        return std::vector<std::string>();
    }
    if ((*_CaData->CalciumConcentrationByIndex_)[0].size() <= 0) {
        _Logger->Log("No Calcium Timesteps Recorded, Skipping Generation, Aborting Render", 6);
        return std::vector<std::string>();
    }
    for (int CalciumConcentrationIndex = 0; CalciumConcentrationIndex < (*_CaData->CalciumConcentrationByIndex_)[0].size(); CalciumConcentrationIndex++) {

        // Now, we enumerate through all the steps needed, one at a time until we reach the end
        for (int XStep = 0; XStep < TotalXSteps; XStep++) {
            for (int YStep = 0; YStep < TotalYSteps; YStep++) {

                // Calculate the filename of the image to be generated, add to list of generated images
                std::string DirectoryPath = "Renders/" + _FilePrefix + "/Slice" + std::to_string(SliceNumber + _SliceOffset) + "/";
                DirectoryPath += "Timestep" + std::to_string(_CaData->CalciumConcentrationTimestep_ms * CalciumConcentrationIndex) + "/"; // fixme - make this done by a list of timesteps instead of a hard-coded single timestep
                double RoundedXCoord = std::ceil(((CameraStepSizeX_um * XStep) + _OffsetX) * 100.0) / 100.0;
                double RoundedYCoord = std::ceil(((CameraStepSizeY_um * YStep) + _OffsetY) * 100.0) / 100.0;
                std::string FilePath = "X" + std::to_string(RoundedXCoord) + "_Y" + std::to_string(RoundedYCoord) + ".png";

                Filenames.push_back(DirectoryPath + FilePath);


                // Setup and submit task to queue for rendering
                std::unique_ptr<ProcessingTask> ThisTask = std::make_unique<ProcessingTask>();
                ThisTask->Array_ = _Array;
                ThisTask->Width_px = _CaData->Params_.ImageWidth_px;
                ThisTask->Height_px = _CaData->Params_.ImageHeight_px;
                ThisTask->VoxelStartingX = VoxelsPerStepX * XStep;
                ThisTask->VoxelStartingY = VoxelsPerStepY * YStep;
                ThisTask->VoxelEndingX = ThisTask->VoxelStartingX + ImageWidth_vox;
                ThisTask->VoxelEndingY = ThisTask->VoxelStartingY + ImageHeight_vox;
                // std::cout<<"StartX:"<<ThisTask->VoxelStartingX<<" StartY:"<<ThisTask->VoxelStartingY<<" EndX:"<<ThisTask->VoxelEndingX<<" EndY:"<<ThisTask->VoxelEndingY<<std::endl;
                ThisTask->VoxelZ = SliceNumber;
                ThisTask->TargetFileName_ = FilePath;
                ThisTask->TargetDirectory_ = DirectoryPath;
                ThisTask->CurrentTimestepIndex_ = CalciumConcentrationIndex;
                ThisTask->CalciumConcentrationByIndex_ = _CaData->CalciumConcentrationByIndex_;
                ThisTask->BrightnessAmplification = _CaData->Params_.BrightnessAmplification;
                ThisTask->AttenuationPerUm = _CaData->Params_.AttenuationPerUm;
                ThisTask->VoxelResolution_um = _CaData->Params_.VoxelResolution_um;
                ThisTask->NumVoxelsPerSlice = _CaData->Params_.NumVoxelsPerSlice;

                _ImageProcessorPool->QueueEncodeOperation(ThisTask.get());
                _CaData->Tasks_.push_back(std::move(ThisTask));




            }
        }

    }

    return Filenames;
}




}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
