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
 * @brief Returns the number of bits per pixel for the given image
 * 
 * @param Image Lucifer image struct
 * @return int Number of bits per pixel
 */
int Lucifer_GetBitsPerPixel(Image &Image);


/**
 * @brief Returns the width of the image from the struct
 * 
 * @param Image Lucifer image struct
 * @return int Width in pixels of the image
 */
int Lucifer_GetWidth(Image &Image);

/**
 * @brief Returns the height of the image from the struct
 * 
 * @param Image Lucifer image struct
 * @return int Height in pixels of the image
 */
int Lucifer_GetHeight(Image &Image);

/**
 * @brief Returns the number of channels in the image
 * 
 * @param Image Lucifer image struct
 * @return int Number of channels (RGB=3, RGBA=4, etc.)
 */
int Lucifer_GetChannes(Image &Image);


/**
 * @brief Copies the image into a new FIBITMAP.
 * You'll need to take care of destroying the returned image manually.
 * 
 * @param Image 
 * @return FIBITMAP* 
 */
FIBITMAP* Lucifer_CreateFIBitmapFromImage(Image &Image);

};