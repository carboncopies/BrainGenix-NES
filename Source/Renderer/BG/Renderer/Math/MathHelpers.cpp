#include <BG/Renderer/State/Scene.h>




namespace BG {
namespace NES {
namespace Renderer {


glm::vec3 Vec3_VSGToGLM(vsg::vec3 _Vector) {
    return glm::vec3(_Vector.x, _Vector.y, _Vector.z);
}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG