#include <BG/Renderer/SceneGraph/GeometryBuilder.h>



namespace BG {
namespace NES {
namespace Renderer {


vsg::ref_ptr<vsg::ProjectionMatrix> CreatePerspectiveMatrix(float _FOV, double _AspectRatio, double _NearClip, double _FarClip) {
    return vsg::Perspective::create(_FOV, _AspectRatio, _NearClip, _FarClip);
}

vsg::ref_ptr<vsg::LookAt> CreateLookAtMatrix(vsg::dvec3 _Position, vsg::dvec3 _Target, vsg::dvec3 _Up) {
    return vsg::LookAt::create(_Position, _Target, _Up);
}

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG