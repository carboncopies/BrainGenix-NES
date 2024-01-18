#include <BG/Renderer/EncoderPool/Image.h>


namespace BG {
namespace NES {
namespace Renderer {


Image::Image() {

}

Image::Image(int _Width, int _Height, int _Channels) {

    Data_ = std::unique_ptr<unsigned char>(new unsigned char[_Width * _Height * _Channels]());

}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG