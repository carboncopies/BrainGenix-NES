//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Menu_Window.h>


GUI_Menu_Window::GUI_Menu_Window(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_Windows* Windows,  ERS_CLASS_VisualRenderer* VisualRendererInstance) {

    SystemUtils_ = SystemUtils;
    VisualRenderer_ = VisualRendererInstance;
    Windows_ = Windows;
    SystemUtils_->Logger_->Log("Editor Setting Up Window Menu", 4);

}


GUI_Menu_Window::~GUI_Menu_Window() {

    SystemUtils_->Logger_->Log("Editor Destroying Window Menu", 4);

}


void GUI_Menu_Window::Draw() {

    // Window Menu
    if (ImGui::BeginMenu("Window")) {



        // Add Windows Menu
        if (ImGui::BeginMenu("Windows")) {

            // Add Scene Tree Editor Window
            ImGui::MenuItem("Scene Tree", "", &Windows_->GUI_Window_SceneTree_->Enabled_);
            ImGui::MenuItem("System Log", "", &Windows_->GUI_Window_SystemLog_->Enabled_);
            ImGui::MenuItem("Asset Explorer", "", &Windows_->GUI_Window_AssetExplorer_->Enabled_);
            ImGui::MenuItem("Object Properties", "", &Windows_->GUI_Window_ObjectProperties_->Enabled_);

            // Framerate Widgets
            if (ImGui::BeginMenu("Framerate")) {

                // Framerate Related Tools
                ImGui::MenuItem("Framerate Counter", "", &Windows_->GUI_Window_FramerateCounter_->Enabled_);
                ImGui::MenuItem("Framerate Histogram", "", &Windows_->GUI_Window_FramerateHistogram_->Enabled_);
                ImGui::MenuItem("Framerate Graph", "", &Windows_->GUI_Window_FramerateGraph_->Enabled_);
                ImGui::MenuItem("Framerate Plot", "", &Windows_->GUI_Window_FrameratePlot_->Enabled_);
                ImGui::MenuItem("Frame Latency Graph", "", &Windows_->GUI_Window_FrameLatencyGraph_->Enabled_);

            ImGui::EndMenu();
            }

            // System Resource Widgets
            if (ImGui::BeginMenu("System Resources")) {

                // Framerate Related Tools
                ImGui::MenuItem("RAM", "", &Windows_->GUI_Window_RAMGraph_->Enabled_);

            ImGui::EndMenu();
            }

      


        ImGui::EndMenu();
        }

        // Viewport Menu
        if (ImGui::BeginMenu("Viewport")) {

            // Viewport Options
            if (ImGui::MenuItem("Add Viewport")) {
                VisualRenderer_->CreateViewport();
            }

            if (ImGui::MenuItem("Remove Viewport")) {
                if (VisualRenderer_->Viewports_.size() > 0) {
                    VisualRenderer_->DeleteViewport(VisualRenderer_->Viewports_.size()-1);
                }
            }
            


        ImGui::EndMenu();
        }

        ImGui::MenuItem("Script Editor", "", &Windows_->GUI_Window_ScriptEditor_->Enabled_);
        ImGui::MenuItem("Shader Editor", "", &Windows_->GUI_Window_ShaderEditor_->Enabled_);


    ImGui::EndMenu();
    }


}