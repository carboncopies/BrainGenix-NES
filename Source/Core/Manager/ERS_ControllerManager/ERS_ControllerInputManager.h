//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

#include <GLFW/glfw3.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ControllerSettings.h>


/**
 * @brief Get and update all joystick input devices, as well as associated controller buttons
 * 
 */
class ERS_CLASS_ControllerInputManager {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_;           /**<Pointer to SystemUtils Struct*/
    std::vector<bool>       IsControllerSupported_; /**<Indicate If Controller Is Supported Or Not*/

    int NumberInputDevices_ = 0;    /**<Current Number Of "Joysticks" Connected*/
    bool FirstFrame_        = true; /**<Only True On First Frame*/



private:

    void UpdateNumberInputDevices();    /**<Updates the number of Input Devices present.*/
    void CheckIfSupportedControllers(); /**<Updates ControllerSupported List*/
    void UpdateControllerStates();      /**<Updates the values for every controller param*/

public:

    int NumberControllers_ = 0; /**<Current Number Of Detected Controllers*/

    std::vector<GLFWgamepadstate>              ControllerStates_;            /**<Vector Of Controller States*/
    std::vector<std::string>                   ControllerNames_;             /**<Human Readable Names Of Controllers*/
    std::vector<ERS_STRUCT_ControllerSettings> ControllerSettings_;          /**<List of controller settings for each controller*/
    std::vector<ERS_STRUCT_ControllerSettings> AvailableControllerProfiles_; /**<List of different loaded profiles. Will try to pick based on correct one automatically*/

    // TODO: add system to apply correct controller profile by name
    // add system to write controller profiles
    // add window to configure profiles
    // add option to add profiles, delete them
    // add optin to save profiles in config window
    // add to project files, so they load with the system

public:

    /**
     * @brief Construct a new ers class ControllerInputManager object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_ControllerInputManager(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Destroy the ers class ControllerInputManager object
     * 
     */
    ~ERS_CLASS_ControllerInputManager();


    /**
     * @brief Call this every frame, updates public member vars such as JS position, buttons, etc.
     * 
     */
    void UpdateControllers();


    /**
     * @brief Run this whenever needed to automatically index new controllers.
     * 
     */
    void DetectControllers();

};