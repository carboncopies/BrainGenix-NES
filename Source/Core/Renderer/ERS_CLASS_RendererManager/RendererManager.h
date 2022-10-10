//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

#include <FreeImage.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>
#include <ERS_CLASS_VisualRenderer.h>
#include <ERS_STRUCT_Camera.h>

#include <ERS_CLASS_ShaderLoader.h>
#include <GUI.h>

#include <ERS_ModelWriter.h>
#include <ERS_CLASS_ModelLoader.h>

#include <ERS_Editor_3DCursor.h>

#include <ERS_STRUCT_IOData.h>
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_HumanInputDeviceUtils.h>
#include <ERS_STRUCT_OpenGLDefaults.h>

#include <ERS_Version.h>




/**
 * @brief This class manages all the rendering systems including the visual, auditory and other sensory rendering systems. 
 * 
 */
class RendererManager {

private:

    ERS_STRUCT_SystemUtils*           SystemUtils_  = nullptr; /**<Raw pointer to SystemUtils that is owned by the main() */
    ERS_STRUCT_ProjectUtils*          ProjectUtils_ = nullptr; /**<Copy Of Project Utils Struct*/
    ERS_STRUCT_HumanInputDeviceUtils* HIDUtils_     = nullptr; /**<Copy Of Pointer To HIDM Struct*/
    GLFWwindow*                       Window_       = nullptr; /**<GLFW Window Instance*/


    std::unique_ptr<ERS_CLASS_VisualRenderer>  VisualRenderer_; /**<Pointer To VisualRenderer Instance*/
    std::unique_ptr<ERS_CLASS_ShaderLoader>    ShaderLoader_;   /**<Pointer To Shader Loader Instnace*/
    std::shared_ptr<ERS_STRUCT_Shader>         Shader_;         /**<ERS Shader Object (Main System Shader)*/
    std::shared_ptr<GUISystem>                 GuiSystem_;      /**<Instance of GUI System*/
    std::unique_ptr<Cursors3D>                 Cursors3D_;      /**<Setup 3D Cursor Class*/
    std::unique_ptr<ERS_STRUCT_OpenGLDefaults> OpenGLDefaults_; /**<Pointer To OpenGL Default Textures/Models*/


    // Control Values
    int         WindowWidth_  = 0;       /**<GLFW Window Width Varaible*/
    int         WindowHeight_ = 0;       /**<GLFW Window Height Varaible*/
    const char* WindowTitle_  = nullptr; /**<GLFW Window Title Variable*/


    /**
     * @brief Initialize And Create A GLFW Window
     * 
     */
    void InitializeGLFW();


    /**
     * @brief Load the editor's data such as the icons and other static images/data. Must be called after setting up an opengl context or a segfault will occur.
     * 
     */
    void LoadEditorData();

    /**
     * @brief Loads an icon and returns the path at the id. Remember to init and de-init freeimage before and after this function.
     * 
     * @param Path 
     * @return unsigned int 
     */
    unsigned int LoadEditorIcon(const char* Path);

    /**
     * @brief This function gets the current opengl error status and if there's an issue it reports it into the log.
     * 
     */
    void ReportOpenGLErrors(); 


public:

    /**
     * @brief Initializes all renderers that have been enabled in the configuration file.
     * 
     * @param SystemConfiguration 
     * @param Logger 
     * @param SystemShouldRun 
     */
    RendererManager(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_STRUCT_HumanInputDeviceUtils* HIDUtils);

    /**
     * @brief Shuts down the renderers and cleans up variables.
     * 
     */
    ~RendererManager();

    /**
     * @brief Called by the main loop, updates all rendered outputs.
     * 
     */
    void UpdateLoop(float DeltaTime);

};