//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#include <Lucifer_WriteImage.h>


namespace Lucifer {

WritingStatus Lucifer_Write(Image& Image, std::unique_ptr<unsigned char[]> *CompressedImageData, unsigned long &Size, ImageFormat Format) {

    // Sanity Checks
    if (Image.Bytes.get() == nullptr) {
        return Lucifer_WritingStatus_InvalidData;
    } else if (Image.Channels < 1 || Image.Channels > 4) {
        return Lucifer_WritingStatus_InvalidNumChannels;
    } else if (Image.Width < 1) {
        return Lucifer_WritingStatus_InvalidWidth;
    } else if (Image.Height < 1) {
        return Lucifer_WritingStatus_InvalidHeight;
    }



    


    // Convert Formats
    FREE_IMAGE_FORMAT FIFormat;
    if (Format == Lucifer_ImageFormat_PNG) {
        FIFormat = FIF_PNG;
    } else if (Format == Lucifer_ImageFormat_JPG) {
        FIFormat = FIF_JPEG;
    } else if (Format == Lucifer_ImageFormat_TIFF) {
        FIFormat = FIF_TIFF;
    }


    // Save Image
    int BitsPP = (Image.Size * 8) / (Image.Width * Image.Height);
    FIBITMAP* FIImage = FreeImage_Allocate(Image.Width, Image.Height, BitsPP, 0, 0, 0);
    memcpy(FreeImage_GetBits(FIImage), Image.Bytes.get(), Image.Width * Image.Height * (BitsPP / 8));

    
    FIMEMORY* Memory = FreeImage_OpenMemory();
    FreeImage_SaveToMemory(FIFormat, FIImage, Memory);

    DWORD ImageCompressedSize = 0;
    BYTE *ImageCompressedBytes;
    FreeImage_AcquireMemory(Memory, &ImageCompressedBytes, &ImageCompressedSize);
    Size = (unsigned long)ImageCompressedSize;


    CompressedImageData->reset(new unsigned char[ImageCompressedSize]);
    memcpy(CompressedImageData->get(), ImageCompressedBytes, ImageCompressedSize);
    FreeImage_CloseMemory(Memory);



    return Lucifer_WritingStatus_Complete;


}



}