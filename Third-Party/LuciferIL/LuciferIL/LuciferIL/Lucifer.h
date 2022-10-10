//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#pragma once

// Std Lib
#include <iostream>

// Internal Lib
#include <LuciferIL/Lucifer_LoadImage.h>
#include <LuciferIL/Lucifer_WriteImage.h>


// Third-Party Lib
#include <FreeImage.h>

//#ifndef _WIN32
//#include <IL/il.h>
//#endif

namespace Lucifer {


class Lucifer {

private:



public:

    /**
     * @brief Create new Lucifer instance, automatically initializes all required image loader functions.
     * 
     */
    Lucifer();

    /**
     * @brief Shut down and destroy the Lucifer Instance.
     * 
     */
    ~Lucifer();


    /**
     * @brief Decodes image data in-memory.
     * 
     * @param ImageData Pointer to raw bytes (these could be read directly off of the disk)
     * @param Size Size of the array
     * @param Image Reference to the image to be updated
     * @param MaxChannels Maximum number of supported channels, defaults to 4
     * @return Lucifer::LoadingStatus Status of loading
     */
    LoadingStatus Load(unsigned char* ImageData, unsigned long Size, Image& Image, int MaxChannels = 4);


    /**
     * @brief Saves the specified image to memory buffer passed in.
     * 
     * @param Image Source image to be saved to memory
     * @param CompressedImageData Unique pointer of buffer to written to 
     * @param Size Size of the buffer in bytes
     * @param Format Format to save the image in
     * @return WritingStatus Status of save operation
     */
    WritingStatus Write(Image& Image, std::unique_ptr<unsigned char[]> *CompressedImageData, unsigned long &Size, ImageFormat Format = Lucifer_ImageFormat_PNG);



};


};
