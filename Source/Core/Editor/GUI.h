//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <implot.h>

#include <ImGuizmo.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>
#include <ERS_CLASS_VisualRenderer.h>

#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_HumanInputDeviceUtils.h>

#include <ERS_Editor_ThemeManager.h>
#include <ERS_Editor_FontManager.h>
#include <ERS_Editor_UserProfileManager.h>
#include <ERS_Editor_3DCursor.h>
#include <ERS_Editor_WindowManager.h>

#include <ERS_SceneManager.h>

#include <GUI_Menu_File.h>
#include <GUI_Menu_Window.h>
#include <GUI_Menu_Debug.h>
#include <GUI_Menu_Settings.h>





/**
 * @brief Implements the ERS gui system.
 * 
 */
class GUISystem {


private:

    GLFWwindow*                       Window_         = nullptr; /**<Pointer to window instance*/
    ERS_STRUCT_SystemUtils*           SystemUtils_    = nullptr; /**<Structure Containing Pointers To System Utilities*/
    ERS_STRUCT_ProjectUtils*          ProjectUtils_   = nullptr; /**ProjectUtils Struct Pointer*/
    Cursors3D*                        Cursors3D_      = nullptr; /**<Pointer to Cursors3D Instance*/
    ERS_CLASS_SceneManager*           SceneManager_   = nullptr; /**<Scene Manager Instance Pointer*/
    ERS_CLASS_VisualRenderer*         VisualRenderer_ = nullptr; /**<Pointer to visual renderer for viewport modification*/
    ERS_STRUCT_HumanInputDeviceUtils* HIDUtils_       = nullptr; /**<Pointer To Human Device Utils Struct*/

    std::unique_ptr<ERS_CLASS_FontManager>        FontManager_;        /**<Pointer To FontManager Instance*/
    std::unique_ptr<ERS_CLASS_UserProfileManager> UserProfileManager_; /**<Pointer To User Profile Manager Instance*/
    std::unique_ptr<ERS_CLASS_WindowManager>      WindowManager_;      /**<Class owning all gui windows*/
    
public:

    std::unique_ptr<ERS_CLASS_ThemeManager> ThemeManager_; /**<Pointer To ThemeManager Instance*/

    // Menu Items
    std::unique_ptr<GUI_Menu_File>     Menu_File_;     /**<Editor Menu Entry*/
    std::unique_ptr<GUI_Menu_Window>   Menu_Window_;   /**<Editor Menu Entry*/
    std::unique_ptr<GUI_Menu_Debug>    Menu_Debug_;    /**<Editor Menu Entry*/
    std::unique_ptr<GUI_Menu_Settings> Menu_Settings_; /**<Editor Menu Entry*/


    /**
     * @brief Construct a new Gui System object
     * 
     * @param Logger 
     */
    GUISystem(ERS_STRUCT_SystemUtils* SystemUtils,
    GLFWwindow* Window,
    Cursors3D* Cursors3D,
    ERS_CLASS_SceneManager* SceneManager,
    ERS_STRUCT_ProjectUtils* ProjectUtils,
    ERS_CLASS_VisualRenderer* VisualRendererInstance,
    ERS_STRUCT_HumanInputDeviceUtils* HIDUtils
    );

    /**
     * @brief Destroy the Gui System object
     * 
     */
    ~GUISystem();


    /**
     * @brief Generate Frame REMEMBER TO CALL UPDATE FRAME AFTER THE CLEAR CALL.
     * 
     */
    void UpdateGUI();

    /**
     * @brief Update Frame, make sure you've called updategui.
     * 
     */
    void UpdateFrame();
    
    /**
     * @brief Use this for functions/widgets/windows that need to be updated after the main rendering takes place.
     * 
     */
    void DeferredFrameUpdate();
};
