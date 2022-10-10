//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#include <Lucifer.h>



namespace Lucifer {


Lucifer::Lucifer() {

    FreeImage_Initialise();

//#ifndef _WIN32
//    ilInit();
//#endif

}

Lucifer::~Lucifer() {

    FreeImage_DeInitialise();

}


LoadingStatus Lucifer::Load(unsigned char* ImageData, unsigned long Size, Image& Image, int MaxChannels) {
    return Lucifer_Load(ImageData, Size, Image, MaxChannels);
}


WritingStatus Lucifer::Write(Image& Image, std::unique_ptr<unsigned char[]> *CompressedImageData, unsigned long &Size, ImageFormat Format) {
    return Lucifer_Write(Image, CompressedImageData, Size, Format);
}




}
