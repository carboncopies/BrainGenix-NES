//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")


/**
 * @brief Used to remap buttons, adjust gains, set thresholds, etc. User adjustable controller settings.
 * 
 */
struct ERS_STRUCT_ControllerSettings {

    // Controller Settings Name
    std::string SettingsProfileName; /**<Name of this profile. Shows in dropdown menu*/
    std::string ControllerName; /**<Name of the controller to apply this profile to*/

    // Joystick Settings
    float MinThresholdJoystickLeftX; /**<Minimum value from joystick to be considered a value*/
    float MinThresholdJoystickLeftY; /**<Minimum value from joystick to be considered a value*/
    float MinThresholdJoystickRightX; /**<Minimum value from joystick to be considered a value*/
    float MinThresholdJoystickRightY; /**<Minimum value from joystick to be considered a value*/

    float MaxThresholdJoystickLeftX; /**<Maximum value from joystick before JS is fully on*/
    float MaxThresholdJoystickLeftY; /**<Minimum value from joystick to be considered a value*/
    float MaxThresholdJoystickRightX; /**<Minimum value from joystick to be considered a value*/
    float MaxThresholdJoystickRightY; /**<Minimum value from joystick to be considered a value*/

    float JoystickLeftXGain; /**<Joystick Gain*/
    float JoystickLeftYGain; /**<Joystick Gain*/
    float JoystickRightXGain; /**<Joystick Gain*/
    float JoystickRightYGain; /**<Joystick Gain*/

    // Trigger Settings
    float MinThresholdTriggerLeft; /**<Minimum value from trigger to be considered valid, all values below this are discarded*/
    float MinThresholdTriggerRight; /**<Minimum value from trigger to be considered valid, all values below this are discarded*/
    
    float MaxThresholdTriggerLeft; /**<Max value for trigger to be fully pressed*/
    float MaxThresholdTriggerRight; /**<Max value for trigger to be fully pressed*/

    // Button Remap
    int TriangleButtonIndex; /**<Button Index*/
    int SquareButtonIndex; /**<Button Index*/
    int CrossButtonIndex; /**<Button Index*/
    int CircleButtonIndex;/**<Button Index*/
    int BackButtonIndex; /**<Button Index*/
    int OptionsButtonIndex; /**<Button Index*/
    int MenuButtonIndex; /**<Button Index*/
    int JoystickRightButtonIndex; /**<Button Index*/
    int JoystickLeftButtonIndex; /**<Button Index*/
    int LeftBumperButtonIndex; /**<Button Index*/
    int RightBumperButtonIndex; /**<Button Index*/
    int DPADUpButtonIndex; /**<Button Index*/
    int DPADDownButtonIndex; /**<Button Index*/
    int DPADLeftButtonIndex; /**<Button Index*/
    int DPADRightButtonIndex; /**<Button Index*/

};