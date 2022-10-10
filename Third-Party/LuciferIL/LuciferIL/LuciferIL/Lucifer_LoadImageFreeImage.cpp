//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#include <Lucifer_LoadImageFreeImage.h>

namespace Lucifer {

LoadingStatus Lucifer_LoadFreeImage(unsigned char* ImageData, unsigned long Size, Image& Image, int MaxChannels) {

    if (ImageData == nullptr) {
        return Lucifer_LoadingStatus_InvalidData;
    }

    FIMEMORY* FIImageData = FreeImage_OpenMemory(ImageData, Size);
    FREE_IMAGE_FORMAT Format = FreeImage_GetFileTypeFromMemory(FIImageData);

    if (Format == FIF_UNKNOWN) {
        FreeImage_CloseMemory(FIImageData);
        return Lucifer_LoadingStatus_UnsupportedFormat;
    }


    FIBITMAP* FIImage = FreeImage_LoadFromMemory(Format, FIImageData);
    FreeImage_CloseMemory(FIImageData);


    // Swap Colors From RGB To BGR
    FIBITMAP* Red = FreeImage_GetChannel(FIImage, FICC_RED);
    FIBITMAP* Green = FreeImage_GetChannel(FIImage, FICC_GREEN);
    FIBITMAP* Blue = FreeImage_GetChannel(FIImage, FICC_BLUE);
    FIBITMAP* Alpha = FreeImage_GetChannel(FIImage, FICC_ALPHA);
    FreeImage_SetChannel(FIImage, Red, FICC_BLUE);
    FreeImage_SetChannel(FIImage, Green, FICC_GREEN);
    FreeImage_SetChannel(FIImage, Blue, FICC_RED);
    FreeImage_SetChannel(FIImage, Alpha, FICC_ALPHA);
    FreeImage_Unload(Red);
    FreeImage_Unload(Green);
    FreeImage_Unload(Blue);
    FreeImage_Unload(Alpha);


    // Get Metadata
    int Width, Height, Line;
    Width  = FreeImage_GetWidth(FIImage);
    Height = FreeImage_GetHeight(FIImage);
    Line  = FreeImage_GetLine(FIImage);

    if (FIImage == nullptr) {
        return Lucifer_LoadingStatus_UnsupportedFormat;
    }    

    
    Image.Bytes.reset(new unsigned char[FreeImage_GetMemorySize(FIImage)]);
    Image.Size = FreeImage_GetMemorySize(FIImage);
    memcpy(Image.Bytes.get(), FreeImage_GetBits(FIImage), FreeImage_GetMemorySize(FIImage));

    Image.Width = Width;
    Image.Height = Height;
    Image.Size = FreeImage_GetMemorySize(FIImage);
    Image.BitsPerPixel = (Image.Size * 8) / (Image.Width * Image.Height);

    if (Image.Width < 1) {
        FreeImage_Unload(FIImage);
        return Lucifer_LoadingStatus_InvalidWidth;
    } else if (Image.Height < 1) {
        FreeImage_Unload(FIImage);
        return Lucifer_LoadingStatus_InvalidHeight;
    }

    Image.Channels = Line / Width;

    if (Image.Channels < 1 || Image.Channels > MaxChannels) {
        FreeImage_Unload(FIImage);
        return Lucifer_LoadingStatus_InvalidNumChannels;
    }

    FreeImage_Unload(FIImage);
    return Lucifer_LoadingStatus_Complete;

}

}