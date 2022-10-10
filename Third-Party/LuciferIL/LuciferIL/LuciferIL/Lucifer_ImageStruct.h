//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#pragma once

#include <memory>


namespace Lucifer {


struct Image {

    int Width        = -1; /**<Width in pixels of the image*/
    int Height       = -1; /**<Height in pixels of the image*/
    int Channels     = -1; /**<Number of channels that the image has (RGB would be 3, RGBA is 4, etc...)*/
    int BitsPerPixel = -1; /**<Number of bits for each pixel. EX: 8 per channel, 3 channels (RGB) = 24 bits per pixel*/

    unsigned long Size;  /**<Number Of Bytes In Image Data*/
    std::unique_ptr<unsigned char[]> Bytes; /**<Pointer to bytes containing the image data*/

};


};