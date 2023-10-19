//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains various helper functions for managing shaders. This is intended to simplify the process of shader creation, usage, and initialization dramatically.
    Additional Notes: None
    Date Created: 2023-09-17
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
#include <mutxex>
#include <vector>

#include <fstream> // temporary include, for testing precompiled shaders - use iosubsystem for this later


// Third-Party Libraries (BG convention: use <> instead of "")
#include <VkBootstrap.h>


#include <shaderc/shaderc.hpp>

// #include <SDL.h>
// #include <SDL_vulkan.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/RenderData.h>



namespace BG {
namespace NES {
namespace Renderer {
namespace Internal {



/**
 * @brief Simple helper function for reading files.
 * 
 * @param _FileName 
 * @return std::vector<char> 
 */
std::vector<char> ReadFile(std::string _FileName);

/**
 * @brief Simple helper function for reading files.
 * 
 * @param _FileName 
 * @return std::string
 */
std::string ReadFileToString(std::string _FileName);


/**
 * @brief First step to compiling GLSL to SPIR-V at runtime for use with Vulkan.
 * This function performs the preoprocessing steps (and probably should be done with one thread per shader (maybe a threadpool?)).
 * To use this function, provide a source string with the glsl to compile, a name of the shader (can just be 'shader_src'), the type of shader
 * such as shaderc_glsl_vertex_shader, and finally a ptr to a string where the result will be put.
 * Returns true on success, false on failure.
 * 
 * @param _Logger Pointer to the logging system, used to log any messages.
 * @param _Source String containing the source GLSL code to be compiled. (Contents of the .vert/.frag/etc file or wherever it's stored.)
 * @param _SourceName Name of the shader (probably just use 'shader_src')
 * @param _ShaderType Type of the shader (ex: vertex, fragment, geometry, compute, etc.) (use shader_glsl_[type]_shader enum)
 * @param _PreprocessedResult Valid ptr to a string where the result is to be put.
 * @param _Verbose Toggle between verbose compiling and non-verbose compiling.
 * @return true 
 * @return false 
 */
bool Shaderc_PreprocessShaderGLSL(BG::Common::Logger::LoggingSystem* _Logger, std::string _Source, std::string _SourceName, shaderc_shader_kind _ShaderType, std::string* _PreprocessedResult, bool _Verbose = true);

/**
 * @brief This is the second step in compiling GLSL to SPIR-V at runtime (at least with the shaderc library).
 * There are two options for the second step. The one you probably should use is the compile to binary option.
 * If you want to see the compiled assembly as a string, this is the otpion for you.
 * You should have already called the preprocess step, which does things like macro substitution and that sort of thing.
 * To use this function, provide a preprocessed source string, the name, type and a pointer to where the result is to be put.
 * If you wish to enable optimizations, set optimize to true.
 * Returns true on success, false on failure.
 * 
 * @param _Logger Pointer to the logging system, used to log any messages.
 * @param _Source tring containing the preprocessed source GLSL code to be compiled.
 * @param _SourceName Name of the shader (probably just use 'shader_src')
 * @param _ShaderType Type of the shader (ex: vertex, fragment, geometry, compute, etc.) (use shader_glsl_[type]_shader enum)
 * @param _CompiledResult Valid ptr to a string where the result is to be put.
 * @param _Optimize Enable or disable compiler optimizations to the shader code.
 * @param _Verbose Toggle between verbose compiling and non-verbose compiling.
 * @return true 
 * @return false 
 */
bool Shaderc_CompileToAssembly(BG::Common::Logger::LoggingSystem* _Logger, std::string _Source, std::string _SourceName, shaderc_shader_kind _ShaderType, std::string* _CompiledResult, bool _Optimize = false, bool _Verbose = true);

/**
 * @brief This is the second step in compiling GLSL to SPIR-V at runtime (at least with the shaderc library).
 * There are two options for compining after preprocessing. This is likely the option you want to use.
 * You should have already called the preprocess step, which does things like macro substitution and that sort of thing.
 * To use this function, provide a preprocessed source string, the name, type and a pointer to where the result is to be put.
 * If you wish to enable optimizations, set optimize to true.
 * Returns true on success, false on failure.
 * 
 * @param _Logger Pointer to the logging system, used to log any messages.
 * @param _Source tring containing the preprocessed source GLSL code to be compiled.
 * @param _SourceName Name of the shader (probably just use 'shader_src')
 * @param _ShaderType Type of the shader (ex: vertex, fragment, geometry, compute, etc.) (use shader_glsl_[type]_shader enum)
 * @param _CompiledResult Valid ptr to a uint32_t vector where the result is to be put.
 * @param _Optimize Enable or disable compiler optimizations to the shader code.
 * @param _Verbose Toggle between verbose compiling and non-verbose compiling.
 * @return true 
 * @return false 
 */
bool Shaderc_CompileToBinary(BG::Common::Logger::LoggingSystem* _Logger, std::string _Source, std::string _SourceName, shaderc_shader_kind _ShaderType, std::vector<uint32_t>* _CompiledResult, bool _Optimize = false, bool _Verbose = true);


/**
 * @brief This is a helper function for vulkan which will compile a GLSL shader string to SPIR-V bytecode in the format of a uint32_t vector.
 * It will handle all intermediary steps automatically. 
 * If you wish to enable optimizations, set optimize to true.
 * Returns true on success, false on failure.
 * 
 * @param _Logger Pointer to the logging system, used to log any messages.
 * @param _Source tring containing the raw source GLSL code to be compiled.
 * @param _SourceName Name of the shader (probably just use 'shader_src')
 * @param _ShaderType Type of the shader (ex: vertex, fragment, geometry, compute, etc.) (use shader_glsl_[type]_shader enum)
 * @param _CompiledResult Valid ptr to a uint32_t vector where the result is to be put.
 * @param _Optimize Enable or disable compiler optimizations to the shader code.
 * @param _Verbose Toggle between verbose compiling and non-verbose compiling.
 * @return true 
 * @return false 
 */
bool Vulkan_DynamicallyCompileShader(BG::Common::Logger::LoggingSystem* _Logger, std::string _Source, std::string _SourceName, shaderc_shader_kind _ShaderType, std::vector<uint32_t>* _CompiledResult, bool _Optimize = false, bool _Verbose = true);


/**
 * @brief Intermediate Vulkan Shader Helper, used to create a shader module from existing SPIR-V bytecode.
 * Returns true on success, false on failure.
 * 
 * @param _Logger Pointer to the logging system, used to log any messages.
 * @param _RD Pointer to system renderdata instance.
 * @param _ShaderBytecode Pointer to vector of characters of the compiled SPIR-V bytecode.
 * @param _ShaderModule Pointer to the VkShaderModule to be populated.
 * @return true 
 * @return false 
 */
bool Vulkan_CreateShaderModule(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD, const std::vector<uint32_t>* _ShaderBytecode, VkShaderModule* _ShaderModule);


}; // Close Namespace Internal






}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG