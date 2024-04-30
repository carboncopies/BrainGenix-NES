
//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines what is imaged and how it's imaged.
    Additional Notes: None
    Date Created: 2023-11-29
    Author(s): Thomas Liao, Randal Koene


    Copyright (C) 2023  Thomas Liao, Randal Koene

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


// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")


namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief Defines a set of parameters used to feed the renderer that specifies what and how to scan something.
 * 
 */
struct MicroscopeParameters {


    float VoxelResolution_um; /**Set the size of each voxel in micrometers*/

    int ImageWidth_px; /**Width of the rendered image in pixels*/
    int ImageHeight_px; /**Height of the rendered image in pixels*/
    float ScanRegionOverlap_percent; /**Percentage of overlap between generated images*/
    float SliceThickness_um; /**How thick each slice is in micrometers*/
    float MicroscopeFOV_deg; /**Field of view of the microscope camera in degrees, we autoposition the height so this doesn't change anything other than the perspective effects.*/
    int NumPixelsPerVoxel_px; /**Sets the size of each voxel in pixels in the fully rendered image (approximately).*/


    bool  GeneratePerlinNoise_ = true; /**Enable or disable perlin noise inside compartments, this is used to simulate cell guts basically*/
    float NoiseIntensity_ = 150; /**How much we scale the noise by*/
    float DefaultIntensity_ = 255; /**What the default color of each compartment is without noise (0-255)*/
    float SpatialScale_ = 10.; /**Set the multiplier for which the x,y,z steps are multiplied by*/

    bool RenderBorders = true; /**Enable or disable border rendering*/
    int BorderEdgeIntensity = 75; /**Set the intensity of the edge of the border*/
    float BorderThickness_um = 0.02; /**Set the thickness of a border line*/

    bool GenerateImageNoise = true; /**Enable or disable the generation of ImageNoise*/
    int ImageNoiseIntensity = 110; /**Arbitrary intensity level*/
    int PreBlurNoisePasses = 1; /**Set the number of times to add noise pre blurring*/
    int PostBlurNoisePasses = 2; /**Set the number of times to add noise pre blurring*/

    bool EnableGaussianBlur = true;  /**Enable or disables gaussian blurring of images*/
    float GaussianBlurSigma = 1.15;   /**Sigma value for amount of blur*/

    bool EnableInterferencePattern = true; /**Enable or disable interference patterns*/
    float InterferencePatternXScale_um = 17.75; /**Set the interference pattern x scale*/
    float InterferencePatternAmplitude = 30.; /**Set the amplitude for the interference pattern*/
    float InterferencePatternBias = -15; /**Set an offset for the patterns generated in terms of color*/


};


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
