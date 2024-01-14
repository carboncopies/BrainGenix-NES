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



namespace BG {
namespace NES {
namespace Renderer {
namespace Shaders {

/**
 * @brief Enum that indicates the type of shader to be used.
 * Various parameters are determined based on the value set here.
 * 
 */
enum ShaderType {
    SHADER_UNDEFINED,
    SHADER_PHONG
};


/**
 * @brief This struct contains information stating how a given object should be shaded.
 * It tells the renderer if it should use a blinn-phong shading model, or something else entirely
 * 
 */
struct Shader {

    ShaderType Type_ = SHADER_UNDEFINED; /**Type of the shader, defined by enum.*/
    
};





}; // Close Namespace Shaders
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG