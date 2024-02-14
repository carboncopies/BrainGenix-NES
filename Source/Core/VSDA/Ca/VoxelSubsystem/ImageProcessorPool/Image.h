
//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the image struct, used to provide work to the encoder pool.
    Additional Notes: None
    Date Created: 2024-01-13
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


// Internal Libraries (BG convention: use <> instead of "")


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {


enum ImageState {
    IMAGE_NOT_PROCESSED=0,
    IMAGE_PROCESSED=1
};


/**
 * @brief Struct containing image data which is used by the encoder to compress and save the image.
 * This is done with as much concurrency as possible by the ImageProcessorPool.
 * 
 */
struct Image {

    int         Width_px;         /**Width of this image in pixels*/
    int         Height_px;        /**Height of this image in pixels*/
    int         TargetWidth_px;   /**Specify the target width of the image in pixels*/
    int         TargetHeight_px;  /**Specify the target height of the image in pixels*/
    int         NumChannels_;     /**Number of channels for this image*/
    std::string TargetFileName_;  /**Filename that this image is to be written to*/

    std::unique_ptr<unsigned char> Data_; /**Pointer to image data to be saved*/

    std::atomic<ImageState> ImageState_ = IMAGE_NOT_PROCESSED; /**State of the image (saved or not yet saved)*/


    /**
     * @brief Construct a new Image object. You'll have to set the data ptr yourself.
     * 
     */
    Image();

    /**
     * @brief Construct a new Image object of specified size.
     * Note - this will *not* initialize the data at the given place.
     * 
     * @param _Width 
     * @param _Height 
     * @param _Channels 
     */
    Image(int _Width, int _Height, int _Channels);


    /**
     * @brief Set the pixel at the given pocation to the given RGB value (0-255)
     * 
     * @param _X X position
     * @param _Y Y position
     * @param _R value from 0-255
     * @param _G value from 0-255
     * @param _B value from 0-255
     */
    void SetPixel(int _X, int _Y, int _R, int _G, int _B);


};




}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
