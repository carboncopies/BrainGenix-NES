//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FUNCTION_ConvertRotationToFrontVector.h>


glm::vec3 ERS_FUNCTION_ConvertRotationToFrontVector(glm::vec3 Rotation) {

    glm::mat4 RotMatrix;
    RotMatrix = glm::rotate(RotMatrix, glm::radians(Rotation[2]), glm::vec3(0, 0, 1));
    RotMatrix = glm::rotate(RotMatrix, glm::radians(Rotation[1]), glm::vec3(0, 1, 0));
    RotMatrix = glm::rotate(RotMatrix, glm::radians(Rotation[0]), glm::vec3(1, 0, 0));

    return glm::vec3(RotMatrix[2][0], RotMatrix[2][1], RotMatrix[2][2]);

}