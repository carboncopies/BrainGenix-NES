//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#pragma once


// Standard Libs
#include <iostream>
#include <memory>
#include <cstring>

// Third-Party Libs
#include <FreeImage.h>

// Internal Libs
#include <LuciferIL/Lucifer_WritingStatusEnum.h>
#include <LuciferIL/Lucifer_ImageFormatEnum.h>
#include <LuciferIL/Lucifer_ImageStruct.h>


namespace Lucifer {


/**
 * @brief Saves the specified image to memory buffer passed in.
 * 
 * @param Image Source image to be saved to memory
 * @param CompressedImageData Unique pointer of buffer to written to 
 * @param Size Size of the buffer in bytes
 * @param Format Format to save the image in
 * @return WritingStatus Status of save operation
 */
WritingStatus Lucifer_Write(Image& Image, std::unique_ptr<unsigned char[]> *CompressedImageData, unsigned long &Size, ImageFormat Format = Lucifer_ImageFormat_PNG);


};