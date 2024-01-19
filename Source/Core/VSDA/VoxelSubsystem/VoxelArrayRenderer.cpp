//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <filesystem>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/VoxelSubsystem/VoxelArrayRenderer.h>





namespace BG {
namespace NES {
namespace Simulator {




bool RenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, VSDAData* _VSDAData, std::vector<std::string>* _FileNameArray, std::string _FilePrefix, int SliceNumber, BG::NES::Renderer::EncoderPool* _EncoderPool) {
    assert(_VSDAData != nullptr);
    assert(_Logger != nullptr);


    // Get Params and Array From VSDAData
    MicroscopeParameters* Params = &_VSDAData->Params_;
    VoxelArray* Array = _VSDAData->Array_.get();


    _Logger->Log(std::string("Rendering Slice '") + std::to_string(SliceNumber) + "'", 1);


    // Setup Voxel Info
    float VoxelSize = Array->GetResolution();
    BoundingBox VoxelBB = Array->GetBoundingBox();
    



    
    // TODO: Add proper total x steps, total y steps

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
    _VSDAData->TotalSliceImages_ = TotalXSteps * TotalYSteps;
    _Logger->Log("Image Stack Will Have " + std::to_string(TotalXSteps) + " X Steps, " + std::to_string(TotalYSteps) + " Y Steps", 4);


    // Now, we enumerate through all the steps needed, one at a time until we reach the end
    for (int XStep = 0; XStep < TotalXSteps; XStep++) {
        for (int YStep = 0; YStep < TotalYSteps; YStep++) {

            // Calculate the filename of the image to be generated
            std::string FilePath = "Renders/" + _FilePrefix + "_Slice" + std::to_string(SliceNumber);
            FilePath += "_X" + std::to_string(CameraStepSizeX_um * XStep) + "_Y" + std::to_string(CameraStepSizeY_um * YStep) + ".png";

            // Next, setup the image and get it ready to be drawn to
            _VSDAData->Images_.push_back(std::make_unique<BG::NES::Renderer::Image>(VoxelsPerStepX, VoxelsPerStepY, NumChannels));
            BG::NES::Renderer::Image* Image = _VSDAData->Images_[_VSDAData->Images_.size() - 1].get();
            Image->TargetHeight_px = _VSDAData->Params_.ImageWidth_px;
            Image->TargetWidth_px = _VSDAData->Params_.ImageHeight_px;
            Image->TargetFileName_ = FilePath;

            // Now enumerate the voxel array and populate the image with the desired pixels (for the subregion we're on)
            int XStartingPosition = VoxelsPerStepX * XStep;
            int YStartingPosition = VoxelsPerStepY * YStep;
            for (unsigned int XVoxelIndex = XStartingPosition; XVoxelIndex < XStartingPosition + VoxelsPerStepX; XVoxelIndex++) {
                for (unsigned int YVoxelIndex = YStartingPosition; YVoxelIndex < YStartingPosition + VoxelsPerStepY; YVoxelIndex++) {

                    // Get Voxel At Position
                    VoxelType ThisVoxel = Array->GetVoxel(XVoxelIndex, YVoxelIndex, SliceNumber);

                    // Now Set The Pixel
                    int ThisPixelX = XVoxelIndex - XStartingPosition;
                    int ThisPixelY = YVoxelIndex - YStartingPosition;
                    if (ThisVoxel == FILLED) {
                        Image->SetPixel(ThisPixelX, ThisPixelY, 220, 220, 220);
                    } else if (ThisVoxel == BORDER) {
                        Image->SetPixel(ThisPixelX, ThisPixelY, 255, 128, 50);
                    } else if (ThisVoxel == OUT_OF_RANGE) {
                        Image->SetPixel(ThisPixelX, ThisPixelY, 0, 0, 255);
                    } else {
                        Image->SetPixel(ThisPixelX, ThisPixelY, 80, 80, 80);
                    }

                }
            }

            _EncoderPool->QueueEncodeOperation(Image);
            _FileNameArray->push_back(FilePath);

            // Update The API Result Info With The Current Slice Number
            _VSDAData->CurrentSliceImage_ = (XStep * TotalYSteps) + YStep + 1;

        }
    }



    return true;
}





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
