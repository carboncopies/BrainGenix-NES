//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains various helper functions for managing shaders. This is intended to simplify the process of shader creation, usage, and initialization dramatically.
    Additional Notes: None
    Date Created: 2023-10-19
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



// Third-Party Libraries (BG convention: use <> instead of "")
#include <shaderc/shaderc.hpp>



// Internal Libraries (BG convention: use <> instead of "")



namespace BG {
namespace NES {
namespace Renderer {
namespace Internal {


/**
 * @brief Internal helper struct, contains all needed work information to compile shader into bytecode.
 * 
 */
struct ShaderCompileObject {

    // Internal Status Flags
    bool ReadyToCompile_             = false; /**Indicates if the struct is fully populated and ready to compile*/
    bool HasCompiled_                = false; /**Indicates if the shader has gone through the compiler*/

    // Input Data
    bool Optimize_                   = false; /**Enable compiler optimizations or disable them*/
    std::string GLSLSource_;                  /**Source string, contains the raw glsl code*/
    std::string SourceName_;                  /**Name of the shader*/
    shaderc_shader_kind ShaderType_;          /**Type of the shader, vertex, fragment, etc.*/

    // Output Data
    shaderc::SpvCompilationResult Result_;    /**End result of the compiled bytecode (SPIR-V)*/


};


}; // Close Namespace Internal
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG