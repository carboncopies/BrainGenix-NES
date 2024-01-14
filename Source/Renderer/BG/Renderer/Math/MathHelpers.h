//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Date Created: 2023-10-30
    Author(s): Thomas Liao


    Copyright (C) 2023  Thomas Liao

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once



// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <assert.h>


// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vsg/all.h>


// Internal Libraries (BG convention: use <> instead of "")




namespace BG {
namespace NES {
namespace Renderer {
namespace Math {


/**
 * @brief Helper function that converts a vsg vector to a glm vector.
 * Basically just reads the x,y,z values of _Vector and creates a glm vector from that. 
 * 
 * @param _Vector VSG Source Vector
 * @return glm::vec3 GLM Vector
 */
glm::vec3 Vec3_VSGToGLM(vsg::vec3 _Vector);

/**
 * @brief Helper function that converts a glm vector to a vsg vector.
 * Basically just reads the x,y,z values of _Vector and creates a vsg vector from that. 
 * 
 * @param _Vector GLM Source Vector
 * @return vsg::vec3 VSG Vector
 */
vsg::vec3 Vec3_GLMToVSG(glm::vec3 _Vector);

/**
 * @brief Converts glm mat4 to vsg mat4.
 * 
 * @param _Matrix Source GLM Matrix
 * @return vsg::mat4 Destination VSG Matrix
 */
vsg::mat4 Mat4_GLMToVSG(glm::mat4 _Matrix);



/**
 * @brief Helper util to create a transformation matrix from the given position, rotation (euler angles), and scale.
 * 
 * @param _Position World-space position
 * @param _Rotation Rotation in degrees (Euler angles)
 * @param _Scale Scale, relative to base size of object
 * @return glm::mat4 Constructed transformation matrix
 */
glm::mat4 BuildTransformMatrix(glm::vec3 _Position, glm::vec3 _Rotation, glm::vec3 _Scale);
vsg::mat4 BuildTransformMatrix(vsg::vec3 _Position, vsg::vec3 _Rotation, vsg::vec3 _Scale);



}; // Close Namespace Math
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG