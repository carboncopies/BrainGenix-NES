//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_HumanInputDeviceUtils.h>
#include <ERS_STRUCT_ControllerSettings.h>
#include <ERS_STRUCT_Windows.h>

#include <ERS_CLASS_VisualRenderer.h>

#include <ERS_Editor_ThemeManager.h>
#include <ERS_Editor_FontManager.h>
#include <ERS_Editor_3DCursor.h>


#include <GUI_Window_About.h>
#include <GUI_Window_AssetExplorer.h>
#include <GUI_Window_ControllerSettings.h>
#include <GUI_Window_FrameLatencyGraph.h>
#include <GUI_Window_FramerateCounter.h>
#include <GUI_Window_FramerateGraph.h>
#include <GUI_Window_FramerateHistogram.h>
#include <GUI_Window_FrameratePlot.h>
#include <GUI_Window_ImportProgressBar.h>
#include <GUI_Window_ObjectProperties.h>
#include <GUI_Window_ProjectSettings.h>
#include <GUI_Window_RAMGraph.h>
#include <GUI_Window_RenderingSettings.h>
#include <GUI_Window_EditorCameraSettings.h>
#include <GUI_Window_SceneTree.h>
#include <GUI_Window_ScriptEditor.h>
#include <GUI_Window_ShaderEditor.h>
#include <GUI_Window_SystemLog.h>
#include <GUI_Window_TestEditor.h>
#include <GUI_Window_ThemeSelector.h>
#include <GUI_Window_FontSelector.h>
#include <GUI_Window_OpenProject.h>
#include <GUI_Window_NewProject.h>
#include <GUI_Window_AssetStreamingSettings.h>

#include <GUI_Window_ImportModel.h>
#include <GUI_Window_ImportModelDirectory.h>


/**
 * @brief This class owns all windows, and instantiates them. 
 * Additionally, it provides functionality for the layout manager to show/hide different windows.
 * Finally, it provides the functionality to draw all windows irrespective of what the menus are doing.
 * 
 */
class ERS_CLASS_WindowManager {

private:

    ERS_STRUCT_SystemUtils*             SystemUtils_ = nullptr; /**<Pointer to System utillities struct, used to get things like logger, etc.*/
    std::unique_ptr<ERS_STRUCT_Windows> Windows_;               /**<Pointer to struct containing all windows setup in this subsystem*/
    std::vector<std::string>            WindowNames_;           /**<List of all understood window names, populated by constructor*/

public:



    /**
     * @brief Construct a new ers class windowmanager object
     * 
     */
    ERS_CLASS_WindowManager(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Destroy the ers class windowmanager object
     * 
     */
    ~ERS_CLASS_WindowManager();


    /**
     * @brief Generate Window Struct, Call As Soon As Possible.
     * 
     */
    void GenerateWindowStruct(ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_STRUCT_HumanInputDeviceUtils* HIDUtils, ERS_CLASS_VisualRenderer* VisualRenderer,
    ERS_CLASS_ThemeManager* ThemeManager, ERS_CLASS_FontManager* FontManager, Cursors3D* Gizmo, ERS_CLASS_SceneManager* SceneManager);


    /**
     * @brief Call this once per frame at the end of the GUI update process - will call the draw function on all windows in the system.
     * 
     */
    void UpdateAllWindows();

    /**
     * @brief Returns a list containing the name of all windows in the system. 
     * These names can be used to query the state of a given window, or set the state of that window.
     * 
     * @return std::vector<std::string> 
     */
    std::vector<std::string> GetWindowNames();

    /**
     * @brief Use this function to get the state of the window in question.
     * State of window is returned by the Status bool pointer.
     * Will return false on error, true on success.
     * 
     * @param WindowName 
     * @param Status
     * @return true 
     * @return false 
     */
    bool GetWindowStatus(std::string WindowName, bool* Status);

    /**
     * @brief Enable or disable a window with this function.
     * Pass in the window's name and the state to set it to.
     * 
     * Returns false on failute, true otherwise. (Ie, if given window name is invalid)
     * 
     * @param WindowName 
     * @param WindowStatus 
     * @return true
     * @return false
     */
    bool SetWindowStatus(std::string WindowName, bool WindowStatus);

    /**
     * @brief Get the struct containing the system's windows.
     * 
     * @return ERS_STRUCT_Windows* 
     */
    ERS_STRUCT_Windows* GetWindowsStruct();

};