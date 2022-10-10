//======================================================================//
// This file is part of the BrainGenix-ERS Environment AssetStreaming System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_RendererSettings.h>

#include <ERS_ENUM_ShadowFilteringType.h>
#include <ERS_ENUM_ShadowUpdateMode.h>



/**
 * @brief Create GUI GUI_Window "AssetStreaming Settings"
 * 
 */
class GUI_Window_AssetStreamingSettings {

private:

    ERS_STRUCT_SystemUtils*  SystemUtils_  = nullptr; /**<System Utils Pointer*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_ = nullptr; /**<Project Utils Struct So We Can Get To The Model Loader*/
    ERS_CLASS_ModelLoader*   ModelLoader_  = nullptr; /**<Model Loading System, We Update This Class And It's Subcomponents To Do Stuff*/


    // Settings
    int TextureStreamingThreads_    = -1;
    int TextureLoadingThreads_      = -1;
    int TextureStreamingQueueLimit_ = -1;

    float VRAMBudgetMiB_  = 0;
    float RAMBudgetMiB_   = 0;
    float VRAMWarningMiB_ = 0;
    float RAMWarningMiB_  = 0;
    

    bool PreventDupeQueueEntries_ = false;
    bool QueuePrioritizationEnabled_ = false;

    int MaxThreads_ = -1;


public:

    bool Enabled_ = true; /**<Show/Hide GUI_Window*/


    /**
     * @brief Construct a new GUI_Window_AssetStreamingSettings object
     * 
     * @param SystemUtils 
     */
    GUI_Window_AssetStreamingSettings(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils);

    /**
     * @brief Destroy the GUI_Window_AssetStreamingSettings object
     * 
     */
    ~GUI_Window_AssetStreamingSettings();


    /**
     * @brief Function that draws the window, should internally check if enable variable is set to true.
     * 
     */
    void Draw();

};