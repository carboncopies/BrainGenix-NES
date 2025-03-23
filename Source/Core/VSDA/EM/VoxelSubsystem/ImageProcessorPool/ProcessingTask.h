
//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the image struct, used to provide work to the encoder pool.
    Additional Notes: None
    Date Created: 2024-01-19
    Author(s): Thomas Liao


    Copyright (C) 2024  Thomas Liao

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once



// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <memory>
#include <atomic>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <noise/noise.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>
#include <VSDA/EM/VoxelSubsystem/Structs/MicroscopeParameters.h>



#define SEGMENTATION_BLOCK_SIZE 2


namespace BG {
namespace NES {
namespace Simulator {


/**
 * @brief Structure that defines the work to be completed. This involves taking a pointer to the voxel array in question,
 * reading the voxels specified, and generating an image with some extra parameters. This is done with multiple threads, 
 * but since we're just reading from the voxelarray, no sync is needed for accessing this data. 
 * 
 */
struct ProcessingTask {
    // virtual ~ProcessingTask() = default; // Add virtual destructor
    std::atomic<bool> IsDone_{false};


    int         Width_px;            /**Width of this image in pixels*/
    int         Height_px;           /**Height of this image in pixels*/
    int         VoxelStartingX;      /**Specify starting x index of the region*/
    int         VoxelStartingY;      /**Specify starting y index of the region*/
    int         VoxelEndingX;        /**Specify the ending x index of the region*/
    int         VoxelEndingY;        /**Specify the ending y index of the region*/
    int         VoxelZ;              /**Specify the slice number that we're going for*/
    int         SliceThickness_vox;  /**Specify the thickness of the slice in voxels*/
    float       VoxelScale_um;       /**Specifies the size of each voxel in microns*/

    bool        EnableImageNoise;    /**Enable or disable image noise*/
    int         ImageNoiseAmount;    /**Arbitrary amount of image noise to add*/
    int         PreBlurNoisePasses;  /**Number of times to add noise prior to blurring*/
    int         PostBlurNoisePasses; /**Number of times to add noise after blurring*/

    bool        EnableGaussianBlur;  /**Enable or disables gaussian blurring of images*/
    float       GaussianBlurSigma;   /**Sigma value for amount of blur*/

    bool EnableInterferencePattern = true; /**Enable or disable interference patterns*/
    float InterferencePatternXScale_um = 1.45; /**Set the interference pattern x scale*/
    float InterferencePatternAmplitude = 40.; /**Set the amplitude for the interference pattern*/
    float InterferencePatternBias = -15; /**Set an offset for the patterns generated in terms of color*/
    float InterferencePatternWobbleFrequency = 3.0f; /**Set the y-axis wobble for the interference pattern*/
    float InterferencePatternYAxisWobbleIntensity = 0.02f; /**Frequency of the interference patterns wobble on the y axis*/
    float InterferencePatternStrengthVariation = 0.12; /**Strength of the wobbles*/
    bool InterferencePatternZOffsetShift = true; /**Enable or disable shifting the interference patterns around between layers*/

    bool AdjustContrast = true; /**Enable or disable the adjustment of contrast*/
    float Contrast = 0.5; /**Values above 1 increase it, values below 1 decrease it*/
    float Brightness = 0; /**Do not change the brightness*/
    float ContrastRandomAmount = 0.1; /**Change the contrast plus or minus this amount*/
    float BrightnessRandomAmount = 0.1; /**Change the brightness per image plus or minus this amount*/

    // std::atomic_bool IsDone_ = false; /**Indicates if this task has been processed or not*/

    std::string TargetFileName_;  /**Filename that this image is to be written to*/
    std::string TargetDirectory_; /**Directory path where the image is to be written to*/

    VoxelArray* Array_ = nullptr;          /**Pointer to the voxel array that we're rendering from*/

    noise::module::Perlin* Generator_ = nullptr; /**Pointer to noise generator */
    MicroscopeParameters* Params_ = nullptr;



    // Segmentation Options
    bool IsSegmentation_;
    std::string OutputPath_;
    std::vector<uint8_t> CompressedData_;
    std::vector<uint64_t> BlockSize_{SEGMENTATION_BLOCK_SIZE, SEGMENTATION_BLOCK_SIZE, SEGMENTATION_BLOCK_SIZE};


};

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
