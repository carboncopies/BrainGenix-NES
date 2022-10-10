//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#include <Lucifer_LoadImageDevIL.h>

#ifndef _WIN32

namespace Lucifer {

LoadingStatus Lucifer_LoadDevIL(unsigned char* ImageData, unsigned long Size, Image& Image, int MaxChannels) {

    if (ImageData == nullptr) {
        return Lucifer_LoadingStatus_InvalidData;
    }

    // Setup Image IDs
    ILuint DevILImageID;
    ilGenImages(1, &DevILImageID);
    ilBindImage(DevILImageID);

    // Load Image
    ILenum ImageFormat = ilDetermineTypeL(ImageData, Size);
    if (ImageFormat == IL_TYPE_UNKNOWN) {
        return Lucifer_LoadingStatus_UnsupportedFormat;
    }
    ilLoadL(ImageFormat, ImageData, Size);

    
    // Get Image Metadata
    Image.Width    = ilGetInteger(IL_IMAGE_WIDTH);
    Image.Height   = ilGetInteger(IL_IMAGE_HEIGHT);
    Image.Channels = ilGetInteger(IL_IMAGE_CHANNELS);

    if (Image.Width < 1) {
        ilDeleteImage(DevILImageID);
        return Lucifer_LoadingStatus_InvalidWidth;
    } else if (Image.Height < 1) {
        ilDeleteImage(DevILImageID);
        return Lucifer_LoadingStatus_InvalidHeight;
    }
    if (Image.Channels < 1 || Image.Channels > MaxChannels) {
        ilDeleteImage(DevILImageID);
        return Lucifer_LoadingStatus_InvalidNumChannels;
    }



    // Memcpy Image Data Pointer
    unsigned long  ImageSize  = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
    unsigned char* ImageBytes = ilGetData();

    if (ImageBytes == nullptr) {
        ilDeleteImage(DevILImageID);
        return Lucifer_LoadingStatus_UnsupportedFormat;
    }    

    Image.Size = ImageSize;
    Image.Bytes.reset(new unsigned char[ImageSize]);
    Image.BitsPerPixel = (Image.Size * 8) / (Image.Width * Image.Height);
    memcpy(Image.Bytes.get(), ImageBytes, ImageSize);


    // Unload And Destroy Image
    ilBindImage(0);
    ilDeleteImage(DevILImageID);

    return Lucifer_LoadingStatus_Complete;

}

}

#endif