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
#include <assert.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>



// Third-Party Libraries (BG convention: use <> instead of "")
#include <shaderc/shaderc.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/Shader/ShaderCompileObject.h>



namespace BG {
namespace NES {
namespace Renderer {
namespace Internal {




/**
 * @brief Multithreaded dynamic shader compiler - can create batches of shaders for compilation with many threads later.
 * Is a wrapper on google's ShaderC compiler which will compile shader source to spir-v bytecode.
 * 
 */
class ShaderCompiler {


private:

    BG::Common::Logger::LoggingSystem *Logger_ = nullptr; /**Logging Class Pointer*/

    // Worker Management Variables
    std::mutex AllowWorkQueueAccess_;                             /**Mutex which blocks access to the work queue so only one thread may use it*/
    std::vector<std::shared_ptr<ShaderCompileObject>> WorkQueue_; /**List of items to be processed by threads*/
    std::atomic_int NextItemToWorkOn_;                            /**During processing, this indicates the next piece to work on*/
    std::atomic_bool KeepThreadsAlive_;                           /**Flag which when set to false tells all worker threads to terminate*/ 
    std::vector<std::thread> WorkerThreads_;                      /**List of worker threads*/


    /**
     * @brief Worker thread function.
     * Created and destroyed by the ProcessWorkQueue Function.
     * 
     */
    void WorkerFunction();

public:

    /**
     * @brief Constructor for the multithreaded shader compiler.
     * This can be done 
    */
    ShaderCompiler(BG::Common::Logger::LoggingSystem* _Logger);

    /**
     * @brief Destructor to the shader compiler.
    */
    ~ShaderCompiler();


    /**
     * @brief Add a given shader to the compiler work queue.
     * Will return the index which can be passed to GetCompiledResult to get the compiled bytecode and status information.
     * Note that Process Work Queue must be called after this, else shaders will not yet be processed.
     * 
     * @param _GLSLSourceCode String containing glsl source
     * @param _SourceName Name of the shader - helps to figure out what this shader does
     * @param _ShaderType Type of the shader (vertex, frag, geometry, compute, etc)
     * @param _Optimize Enable or disable optimizations to the shader
     * @return int Integer index which can be used to get the result after processing
     */
    int AppendShaderToWorkQueue(std::string _GLSLSourceCode, std::string _SourceName, shaderc_shader_kind _ShaderType, bool _Optimize = false);

    /**
     * @brief Processes the existing work queue which has been built from calls to AppendShaderToWorkQueue()
     * Will use the number of threads specified to process the shaders.
     * Blocks until all shaders are done compiling.
     * 
     * @param _NumThreads Specify the number of threads to use.
     */
    void ProcessWorkQueue(int _NumThreads);

    /**
     * @brief Retrieve compiled shader code from the system and push that data into the _DestinationVector.
     * Returns true on success, false if there was an error.
     * 
     * @param _ShaderID Index of the shader given during the AppendShaderToQorkQueue function
     * @param _DestinationVector Vector of uint32_t where the bytecode is to be put
     * @return true 
     * @return false 
     */
    bool GetShaderBytecode(int _ShaderID, std::vector<uint32_t>* _DestinationVector);

    /**
     * @brief Will clear the work queue and invalidatate all Work IDs.
     * Call this before appending to the work queue again (after processing).
     * 
     */
    void ResetWorkQueue();


};


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