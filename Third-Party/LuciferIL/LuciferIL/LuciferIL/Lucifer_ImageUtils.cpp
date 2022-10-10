//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#include <Lucifer_WriteImage.h>


namespace Lucifer {

int Lucifer_GetBitsPerPixel(Image &Image) {
    return Image.BitsPerPixel;
}
int Lucifer_GetWidth(Image &Image) {
    return Image.Width;
}
int Lucifer_GetHeight(Image &Image) {
    return Image.Height;
}
int Lucifer_GetChannels(Image &Image) {
    return Image.Channels;
}

FIBITMAP* Lucifer_CreateFIBitmapFromImage(Image &Image) {
    
    // Allocate New Image
    int BitsPP = (Image.Size * 8) / (Image.Width * Image.Height);
    FIBITMAP* FIImage = FreeImage_Allocate(Image.Width, Image.Height, BitsPP);

    // Copy Image Pixel Data To FIBITMAP
    int ImageCalculatedSize = Image.Width * Image.Height * (BitsPP / 8);
    int FreeImageAllocatedSize = FreeImage_GetMemorySize(FIImage);
    int ActualSize = Image.Size;

    int MallocSize = ImageCalculatedSize;
    MallocSize = std::min(MallocSize, FreeImageAllocatedSize);
    MallocSize = std::min(MallocSize, ActualSize);

    memcpy(FreeImage_GetBits(FIImage), Image.Bytes.get(), MallocSize);

    return FIImage;

    

}


}