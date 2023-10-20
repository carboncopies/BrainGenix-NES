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
#include <functional>
#include <mutex>
#include <atomic>
#include <vector>



// Third-Party Libraries (BG convention: use <> instead of "")
#include <VkBootstrap.h>

#include <shaderc/shaderc.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/RenderData.h>

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



}; // Close Namespace Internal
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG