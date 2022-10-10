//-----------------------//
// Lucifer Image Library //
// Thomas Liao (2022)    //
//-----------------------//

#include <Lucifer_LoadImage.h>

namespace Lucifer {

LoadingStatus Lucifer_Load(unsigned char* ImageData, unsigned long Size, Image& Image, int MaxChannels) {

    if (ImageData == nullptr) {
        return Lucifer_LoadingStatus_InvalidData;
    }

    LoadingStatus Status;

    Status = Lucifer_LoadSTB(ImageData, Size, Image, MaxChannels);
    if (Status == Lucifer_LoadingStatus_Complete) {
        return Status;
    }
    Status = Lucifer_LoadFreeImage(ImageData, Size, Image, MaxChannels);
    if (Status == Lucifer_LoadingStatus_Complete) {
        return Status;
    }

//#ifndef _WIN32

//    Status = Lucifer_LoadDevIL(ImageData, Size, Image, MaxChannels);
//    if (Status == Lucifer_LoadingStatus_Complete) {
//        return Status;
//    }

//#endif

    return Status;

}

}
