//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_ControllerSettings.h>


GUI_Window_ControllerSettings::GUI_Window_ControllerSettings(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_HumanInputDeviceUtils* HIDUtils, ERS_STRUCT_ProjectUtils* ProjectUtils) {

    SystemUtils_ = SystemUtils;
    HIDUtils_ = HIDUtils;
    ProjectUtils_ = ProjectUtils;

}

GUI_Window_ControllerSettings::~GUI_Window_ControllerSettings() {

}


void GUI_Window_ControllerSettings::ButtonText(std::string ButtonName, bool ButtonState) {

    // Generate Text
    std::string ButtonStateLabel = "Released";
    if (ButtonState) {
        ButtonStateLabel = "Pressed";
    }

    ImGui::Text("%s", (ButtonName + std::string(": ") + ButtonStateLabel).c_str());
}

void GUI_Window_ControllerSettings::Draw() {


    if (Enabled_) {
    bool Visible = ImGui::Begin("Game Controller Settings", &Enabled_);

        // Set Window Size
        ImGui::SetWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);


        if (Visible) {
            


            // Begin Tabs For Info, Config
            if (ImGui::BeginTabBar("Controller Settings Tab Bar")) {


                // Info Tab
                if (ImGui::BeginTabItem("Info")) {



                    // Active Selected Controller Dropdown
                    int NumberControllers = HIDUtils_->ControllerInputManager->NumberControllers_;
                    for (int i = 0; i < NumberControllers; i++) {
                        ControllerNames_[i] = HIDUtils_->ControllerInputManager->ControllerNames_[i].c_str();
                    }
                    ImGui::Combo("Selected Controller", &SelectedController_, ControllerNames_, NumberControllers, NumberControllers);
                    ImGui::Separator();


                    // Info Child Window
                    ImGui::BeginChild("Controller Info");

                    // Check if there aren't any controllers, display no controllers message
                    if (NumberControllers == 0) {
                        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "No controllers detected. Try running 'Detect New Controllers'");
                    } else {

                        // Get Game Controller State
                        GLFWgamepadstate State = HIDUtils_->ControllerInputManager->ControllerStates_[SelectedController_];

                        // Joystick Info
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Joysticks");
                        ImGui::Separator();

                        // Joystick Positions
                        ImGui::Text("Left Joystick Vertical Axis: %f", State.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
                        ImGui::Text("Left Joystick Horizontal Axis: %f", State.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
                        
                        ImGui::Text("Right Joystick Vertical Axis: %f", State.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
                        ImGui::Text("Right Joystick Horizontal Axis: %f", State.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);

                        // Joysticks Pressed
                        ButtonText(std::string("Left Joystick"), State.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB]);
                        ButtonText(std::string("Right Joystick"), State.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB]);

                        // Trigger Info
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Triggers");
                        ImGui::Separator();

                        ImGui::Text("Left Trigger: %f", State.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]);
                        ImGui::Text("Right Trigger: %f", State.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]);

                        ButtonText("Left Bumper", State.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]);
                        ButtonText("Right Bumper", State.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER]);


                        // DPAD Info
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "DPAD");
                        ImGui::Separator();

                        ButtonText("DPAD Up", State.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]);
                        ButtonText("DPAD Down", State.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]);
                        ButtonText("DPAD Left", State.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]);
                        ButtonText("DPAD Right", State.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]);


                        // TRIANGLE/CIRCLE/SQUARE/X Buttons
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Input Buttons");
                        ImGui::Separator();

                        ButtonText("Triangle", State.buttons[GLFW_GAMEPAD_BUTTON_TRIANGLE]);
                        ButtonText("Square", State.buttons[GLFW_GAMEPAD_BUTTON_SQUARE]);
                        ButtonText("Circle", State.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE]);
                        ButtonText("Cross", State.buttons[GLFW_GAMEPAD_BUTTON_CROSS]);
                        
                        ButtonText("Back", State.buttons[GLFW_GAMEPAD_BUTTON_BACK]);
                        ButtonText("Menu", State.buttons[GLFW_GAMEPAD_BUTTON_GUIDE]);

                    }
                    ImGui::EndChild();

                ImGui::EndTabItem();
                }

                // Settings Tab
                if (ImGui::BeginTabItem("Settings")) {


                    // Get Controller Settings Info
                    std::vector<ERS_STRUCT_ControllerSettings>* ControllerSettings = ProjectUtils_->ProjectManager_->Project_.ControllerSettings;

                    // ADD DEFAULT CONTROLLER LAYOUTS (IN PROJECT)
                    // ADD CONTROLLER SETTINGS BELOW (THIS FILE)
                    // ADD CONTROLLER SETTINGS WRITER
                    // IMPLEMENT CONTROLLER PROFILE SAVING WHEN PROJECT IS SAVED
                    // ADD VIEWPORT SELECTION?
                    // ADD SUPPORT TO ACTUALLY USE CONTROLLER SETTINGS


                    // Update Controller Dropdown List And Index
                    if ((long)SelectedControllerProfile_ > (long)ControllerSettings->size()) {
                        SelectedControllerProfile_ = (int)ControllerSettings->size() - 1;
                    }

                    for (int i = 0; (long)i < (long)ControllerSettings->size(); i++) {
                        ControllerProfileNames_[i] = (*ControllerSettings)[i].SettingsProfileName.c_str();
                    }

                    // Selector Dropdown
                    if (ControllerSettings->size() > 0) {
                        if (ImGui::Button("Delete Profile")) {
                            ProjectUtils_->ProjectManager_->Project_.ControllerSettings->erase(ProjectUtils_->ProjectManager_->Project_.ControllerSettings->begin() + SelectedControllerProfile_);
                            ProjectUtils_->ProjectManager_->Project_.GameControllerSettingsIDs.erase(ProjectUtils_->ProjectManager_->Project_.GameControllerSettingsIDs.begin() + SelectedControllerProfile_);
                            SelectedControllerProfile_--;
                        }
                    }
                    if (ControllerSettings->size() < 128) {
                        ImGui::SameLine();
                        if (ImGui::Button("Add Profile")) {
                            ProjectUtils_->ProjectManager_->Project_.GameControllerSettingsIDs.push_back(SystemUtils_->ERS_IOSubsystem_->AllocateAssetID());
                            ProjectUtils_->ProjectManager_->Project_.ControllerSettings->push_back(ERS_STRUCT_ControllerSettings());
                        }
                    }
                    ImGui::SameLine();
                    ImGui::Combo("Selected Controller Profile", &SelectedControllerProfile_, ControllerProfileNames_, ControllerSettings->size(),  ControllerSettings->size());

                    ImGui::Separator();



                    // Child Settings
                    ImGui::BeginChild("Controller Setttings"); 


                    
                    // Check if there aren't any controller profiles, display no controllers message
                    if (ControllerSettings->size() == 0) {
                        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "No controller profiles found. Try adding a new one.");
                    } else {

                        // Thresholds
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Joystick Thresholds");
                        ImGui::Separator();

                    }

                    ImGui::EndChild();

                ImGui::EndTabItem();
                }


            ImGui::EndTabBar();
            }

        }

    ImGui::End();
    }


}