//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#include <Lucifer_LoadImageSTB.h>


namespace Lucifer {

LoadingStatus Lucifer_LoadSTB(unsigned char* ImageData, unsigned long Size, Image& Image, int MaxChannels) {

    // Sanity Check On Input
    if (ImageData == nullptr) {
        return Lucifer_LoadingStatus_InvalidData;
    }

    // Decode Image In Memory
    int Width, Height, Channels;
    unsigned char* ImageBytes = stbi_load_from_memory(ImageData, Size, &Width, &Height, &Channels, 0);
    if (ImageBytes == nullptr) {
        return Lucifer_LoadingStatus_UnsupportedFormat;
    }


    // Copy Image Data
    int ImageLength = Width*Height*Channels;//= strlen((const char*)ImageBytes);

    Image.Bytes.reset(new unsigned char[ImageLength]);
    memcpy(Image.Bytes.get(), ImageBytes, ImageLength);


    Image.Width = Width;
    Image.Height = Height;
    Image.Channels = Channels;
    Image.Size = ImageLength;
    Image.BitsPerPixel = 8;

    stbi_image_free(ImageBytes);
    
    // More Sanity Checking
    if (Image.Width < 1) {
        return Lucifer_LoadingStatus_InvalidWidth;
    } else if (Image.Height < 1) {
        return Lucifer_LoadingStatus_InvalidHeight;
    } else if (Image.Channels < 1 || Image.Channels > MaxChannels) {
        return Lucifer_LoadingStatus_InvalidNumChannels;
    }
    return Lucifer_LoadingStatus_Complete;

}

}