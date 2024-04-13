#include <BG/Renderer/Math/MathHelpers.h>




namespace BG {
namespace NES {
namespace Renderer {
namespace Math {

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

glm::mat4 BuildTransformMatrix(glm::vec3 _Position, glm::vec3 _Rotation, glm::vec3 _Scale) {

    // Create Initial Matrix, Transform To Correct Position
    glm::mat4 TransformationMatrix = glm::translate(glm::mat4(1.0f), _Position);

    // Rotate Transform Matrix On Z,X,Y Axis
    TransformationMatrix = glm::rotate(TransformationMatrix, glm::radians(_Rotation.z), glm::vec3(0, 0, 1));
    TransformationMatrix = glm::rotate(TransformationMatrix, glm::radians(_Rotation.y), glm::vec3(0, 1, 0));
    TransformationMatrix = glm::rotate(TransformationMatrix, glm::radians(_Rotation.x), glm::vec3(1, 0, 0));

    // Scale Transform Matrix Accordingly
    TransformationMatrix = glm::scale(TransformationMatrix, _Scale);

    return TransformationMatrix;
}

vsg::mat4 BuildTransformMatrix(vsg::vec3 _Position, vsg::vec3 _Rotation, vsg::vec3 _Scale) {
    glm::vec3 Pos = Vec3_VSGToGLM(_Position);
    glm::vec3 Rot = Vec3_VSGToGLM(_Rotation);
    glm::vec3 Scale = Vec3_VSGToGLM(_Scale);
    
    return Mat4_GLMToVSG(BuildTransformMatrix(Pos, Rot, Scale));
}


}; // Close Namespace Math
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG