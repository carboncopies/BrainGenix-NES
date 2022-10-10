//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Menu_Debug.h>


// Constructor
GUI_Menu_Debug::GUI_Menu_Debug(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_Windows* Windows, ERS_CLASS_WindowManager* WindowManager) {

    SystemUtils_ = SystemUtils;
    Windows_ = Windows;
    WindowManager_ = WindowManager;
    SystemUtils_->Logger_->Log("Editor Setting Up Debug Menu", 4);

    SystemUtils_->Logger_->Log("Reading Configuration File For 'ShowEditorDebugMenu' Parameter", 1);
    DebugMenuEnabled_ = (*SystemUtils_->LocalSystemConfiguration_)["ShowEditorDebugMenu"].as<bool>();

    // Setup OpenGL Debug Submenu
    ERS_CLASS_OpenGLDebug_ = std::make_unique<ERS_CLASS_OpenGLDebug>(SystemUtils_);
}


// Destructor
GUI_Menu_Debug::~GUI_Menu_Debug() {

    // Log Destructor
    SystemUtils_->Logger_->Log("Editor Destroying Debug Menu", 4);

}


// Draw Function
void GUI_Menu_Debug::Draw() {

    // If Enabled
    if (DebugMenuEnabled_) {

        // Debug Menu
        if (ImGui::BeginMenu("Debug")) {


            // Debugging Tools Menu
            if (ImGui::MenuItem("ImGui Demo Window")) {
                ShowImGuiDemoWindow_ = !ShowImGuiDemoWindow_;
            }

            // Test Editor
            ImGui::MenuItem("Test Editor Window", "", &Windows_->GUI_Window_TestEditor_->Enabled_);



            ImGui::Separator();

            // Window Manager Debug stuff - show, hide, toggle windows.
            if (ImGui::BeginMenu("Window Manager Debugging")) {

                if (ImGui::MenuItem("Show All Windows")) {

                    std::vector<std::string> WindowNames = WindowManager_->GetWindowNames();
                    for (unsigned int i = 0; i < WindowNames.size(); i++) {
                        bool Status = WindowManager_->SetWindowStatus(WindowNames[i], true);
                        if (!Status) {
                            SystemUtils_->Logger_->Log(std::string("Warning, WindowManager Window '") + WindowNames[i] + std::string("' Invalid, Check WindowManager Class For Errors In Code"), 10);
                        }
                    }

                }

                if (ImGui::MenuItem("Hide All Windows")) {

                    std::vector<std::string> WindowNames = WindowManager_->GetWindowNames();
                    for (unsigned int i = 0; i < WindowNames.size(); i++) {
                        bool Status = WindowManager_->SetWindowStatus(WindowNames[i], false);
                        if (!Status) {
                            SystemUtils_->Logger_->Log(std::string("Warning, WindowManager Window '") + WindowNames[i] + std::string("' Invalid, Check WindowManager Class For Errors In Code"), 10);
                        }
                    }

                }

                if (ImGui::MenuItem("Invert Window States")) {

                    std::vector<std::string> WindowNames = WindowManager_->GetWindowNames();
                    for (unsigned int i = 0; i < WindowNames.size(); i++) {
                        bool WindowState;
                        bool Status = WindowManager_->GetWindowStatus(WindowNames[i], &WindowState);
                        if (!Status) {
                            SystemUtils_->Logger_->Log(std::string("Warning, WindowManager Window '") + WindowNames[i] + std::string("' Invalid, Check WindowManager Class For Errors In Code"), 10);
                        }
                        WindowManager_->SetWindowStatus(WindowNames[i], !WindowState);
                    }

                }

            ImGui::EndMenu();
            }



            // OpenGL Debug Submenu
            if (ImGui::BeginMenu("OpenGL Debugging")) {
                ERS_CLASS_OpenGLDebug_->DrawMenu();
            ImGui::EndMenu();
            }


        ImGui::EndMenu();
        }


        // Show Demo Window If Enabled
        if (ShowImGuiDemoWindow_) {
            ImGui::ShowDemoWindow(&ShowImGuiDemoWindow_);
        }

    }

}