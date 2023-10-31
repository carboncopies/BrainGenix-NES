#include <BG/Renderer/Math/MathHelpers.h>




namespace BG {
namespace NES {
namespace Renderer {


glm::vec3 Vec3_VSGToGLM(vsg::vec3 _Vector) {
    return glm::vec3(_Vector.x, _Vector.y, _Vector.z);
}

vsg::vec3 Vec3_GLMToVSG(glm::vec3 _Vector) {
    return vsg::vec3(_Vector.x, _Vector.y, _Vector.z);
}

vsg::mat4 Mat4_GLMToVSG(glm::mat4 _Matrix) {
    vsg::mat4 Output;
    float* FloatArr = (float*)glm::value_ptr(_Matrix);

    Output.set(
        FloatArr[0], FloatArr[1], FloatArr[2],
        FloatArr[3], FloatArr[4], FloatArr[5],
        FloatArr[6], FloatArr[7], FloatArr[8],
        FloatArr[9], FloatArr[10], FloatArr[11],
        FloatArr[12], FloatArr[13], FloatArr[14],
        FloatArr[15]);

    return Output;
}

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG