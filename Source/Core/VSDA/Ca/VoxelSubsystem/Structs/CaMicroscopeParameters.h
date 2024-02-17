
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
#include <vector>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {

/**
 * @brief Defines a set of parameters used to feed the renderer that specifies what and how to scan something.
 * 
 */
struct CaMicroscopeParameters {


    float VoxelResolution_um;               /**Set the size of each voxel in micrometers*/
    int ImageWidth_px;                      /**Width of the rendered image in pixels*/
    int ImageHeight_px;                     /**Height of the rendered image in pixels*/
    float ScanRegionOverlap_percent;        /**Percentage of overlap between generated images*/
    int NumPixelsPerVoxel_px;               /**Sets the size of each voxel in pixels in the fully rendered image (approximately).*/
    int NumVoxelsPerSlice;                  /**Number of voxels deep we're scanning*/
    
    std::vector<int> FlourescingNeuronIDs_; /**List of neuron IDs that flouresce, all others are ignored*/
    std::string CalciumIndicator_;          /**Name of the calcium indicator currently being used*/
    float IndicatorRiseTime_ms;             /**Rise time of the indicator in milliseconds (how long before it sarts glowing)*/
    float IndicatorDecayTime_ms;            /**Decay of the time of the indicator in milliseconds (how long before it stops glowing)*/
    float IndicatorInterval_ms;             /**Interval of update for the indicator in milliseconds*/
    float ImagingInterval_ms;               /**Interval at which Ca images are produced, typically on the same order as IndicatorDecayTime_ms*/
    float AttenuationPerUm;                 /** something*/

    float BrightnessAmplification;          /**This tunes the output amplification for fluorescence imaging*/

};


}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
