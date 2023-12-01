#include <BG/Renderer/SceneGraph/GeometryBuilder.h>



namespace BG {
namespace NES {
namespace Renderer {


vsg::ref_ptr<vsg::ProjectionMatrix> CreatePerspectiveMatrix(float _FOV, double _AspectRatio, double _NearClip, double _FarClip) {
    return vsg::Perspective::create(_FOV, _AspectRatio, _NearClip, _FarClip);
}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG