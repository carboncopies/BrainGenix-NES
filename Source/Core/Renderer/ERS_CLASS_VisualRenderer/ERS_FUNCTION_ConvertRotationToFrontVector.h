//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Internal Libraries (BG convention: use <> instead of "")


/**
 * @brief Converts the passed in rotation to the front vector.
 * 
 * @param Rotation 
 * @return glm::vec3 
 */
glm::vec3 ERS_FUNCTION_ConvertRotationToFrontVector(glm::vec3 Rotation);