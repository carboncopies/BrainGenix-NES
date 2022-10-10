//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <memory>
#include <thread>
#include <string>
#include <mutex>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_HumanInputDeviceUtils.h>
#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_ControllerSettings.h>




/**
 * @brief Window for the Import progress bar.
 * 
 */
class GUI_Window_ControllerSettings {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Pointers To System Resources*/
    ERS_STRUCT_HumanInputDeviceUtils* HIDUtils_; /**<Pointer To Human Input Device Info*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_; /**<Project Utils Pointer Struct*/


    const char* ControllerNames_[16]; /**<List Of Controller Names*/
    int SelectedController_ = 0; /**<Active Selected Controller*/
    const char* ControllerProfileNames_[128]; /**<List of Controller Profile Names*/
    int SelectedControllerProfile_ = 0; /**<Index Of Selected Controller Profile*/


private:
    
    /**
     * @brief Draws ImGui Text For Pressed/Released
     * 
     * @param ButtonName 
     * @param ButtonState 
     */
    void ButtonText(std::string ButtonName, bool ButtonState);

public:

    bool Enabled_ = false; /**<Is Popup Enabled*/


public:

    /**
     * @brief Construct a new Window_ControllerSettings object
     * 
     */
    GUI_Window_ControllerSettings(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_HumanInputDeviceUtils* HIDUtils, ERS_STRUCT_ProjectUtils* ProjectUtils);
    
    /**
     * @brief Destroy the Window_ControllerSettings object
     * 
     */
    ~GUI_Window_ControllerSettings();


    /**
     * @brief Draw import bar if needed (call every frame)
     * 
     */
    void Draw();

};