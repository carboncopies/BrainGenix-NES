//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//


#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <string>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>
#include <glad/glad.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>

#include <ERS_OpenGLLoggingSystem.h>



/**
 * @brief This class provides functionality to the menu including the Debugging and Profiling tools as provided by OpenGL.
 * 
 */
class ERS_CLASS_OpenGLDebug {

private:

    ERS_STRUCT_SystemUtils*                  SystemUtils_ = nullptr;   /**<SystemUtils Instance, Used To Get Systems From Other Classes*/
    std::unique_ptr<ERS_OpenGLLoggingSystem> ERS_OpenGLLoggingSystem_; /**<Logging System Used To Get OpenGL Callback Data*/

    bool DebugEnabled_ = false; /**<Used To Toggle On Or Off The Associated GL_DEBUG_OUTPUT Enum*/


public:

    /**
     * @brief Construct a new ers class opengldebug object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_OpenGLDebug(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Destroy the ers class opengldebug object
     * 
     */
    ~ERS_CLASS_OpenGLDebug();


    /**
     * @brief Draws the menu into the debug menu when called
     * 
     */
    void DrawMenu();

};