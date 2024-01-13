//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Date Created: 2023-10-26
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
#include <vsg/all.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Renderer/SceneGraph/Shader/Shader.h>



namespace BG {
namespace NES {
namespace Renderer {
namespace Primitive {



/**
 * @brief This struct contains the data needed to create a Sphere.
 * It's intended to be populated by a calling function and then passed into a GeometryBuilder instance.
 * From there, it will create a sphere with the defined values. 
 * 
 */
struct Sphere {

    // Physical Attributes
    vsg::vec3 Position_; /**Position of the sphere from the origin of the world*/
    float Radius_;       /**Radius of the sphere*/

    Shaders::Shader* Shader_ = nullptr; /**Defines the info/type of shader to be used for this cube*/

};




}; // Close Namespace State
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG