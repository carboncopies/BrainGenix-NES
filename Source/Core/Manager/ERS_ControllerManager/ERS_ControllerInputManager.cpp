//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_ControllerInputManager.h>



ERS_CLASS_ControllerInputManager::ERS_CLASS_ControllerInputManager(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;

    SystemUtils_->Logger_->Log("Initializing ERS Controller Input Manager", 5);

    DetectControllers();


}

ERS_CLASS_ControllerInputManager::~ERS_CLASS_ControllerInputManager() {

    SystemUtils_->Logger_->Log("ERS Controller Input Manager Destructor Called", 6);

}


void ERS_CLASS_ControllerInputManager::UpdateNumberInputDevices() {

    // Iterate Through All 16 Supported Controllers
    NumberInputDevices_ = 0;
    for (int i = 0; i < 16; i++) {
        if(glfwJoystickPresent(i)) {
            NumberInputDevices_ ++;
        }
    }

}

void ERS_CLASS_ControllerInputManager::CheckIfSupportedControllers() {

    // Clear Controller Supported List
    IsControllerSupported_.erase(IsControllerSupported_.begin(), IsControllerSupported_.end());

    // Iterate Through Current Number Controllers
    for (int i = 0; i < NumberInputDevices_; i++) {
        IsControllerSupported_.push_back(glfwJoystickIsGamepad(i));
    }

}

void ERS_CLASS_ControllerInputManager::UpdateControllerStates() {

    // Clear States
    ControllerStates_.erase(ControllerStates_.begin(), ControllerStates_.end());
    ControllerNames_.erase(ControllerNames_.begin(), ControllerNames_.end());
    NumberControllers_ = 0;

    // Iterate Through Joysticks, Check If Controller
    for (int i = 0; i < NumberInputDevices_; i++) {

        if (IsControllerSupported_[i]) {

            // Incriment Number Of Controller Info
            NumberControllers_++;

            // Get Controller Info
            GLFWgamepadstate State;
            glfwGetGamepadState(i, &State);
            ControllerStates_.push_back(State);

            // Append To Name
            ControllerNames_.push_back(std::string(glfwGetGamepadName(i)));

            // Add Default Settings
            ControllerSettings_.push_back(ERS_STRUCT_ControllerSettings());

        }

    }


}


void ERS_CLASS_ControllerInputManager::UpdateControllers() {

    // On First Frame, Detect Controllers
    if (FirstFrame_) {
        DetectControllers();
        FirstFrame_ = false;
    }

    // Update Data
    UpdateControllerStates();

}


void ERS_CLASS_ControllerInputManager::DetectControllers() {

    // Update Data
    UpdateNumberInputDevices();
    CheckIfSupportedControllers();

}