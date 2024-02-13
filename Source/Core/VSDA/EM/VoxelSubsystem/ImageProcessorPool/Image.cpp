//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/Image.h>


namespace BG {
namespace NES {
namespace Simulator {


Image::Image() {

}

Image::Image(int _Width, int _Height, int _Channels) {

    Data_ = std::unique_ptr<unsigned char>(new unsigned char[_Width * _Height * _Channels]());

    Width_px = _Width;
    Height_px = _Height;
    NumChannels_ = _Channels;

}

void Image::SetPixel(int _X, int _Y, int _R, int _G, int _B) {

    size_t Position = ((_X  + ((Width_px) * _Y)) * NumChannels_);

    Data_.get()[Position + 0] = (unsigned char)_R;
    Data_.get()[Position + 1] = (unsigned char)_G;
    Data_.get()[Position + 2] = (unsigned char)_B;

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG