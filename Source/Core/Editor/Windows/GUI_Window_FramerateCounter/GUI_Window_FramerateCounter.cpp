//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_FramerateCounter.h>


void GUI_Window_FramerateCounter::Draw() {

    // If Window Drawing Enabled
    if (Enabled_) {
        bool Visible = ImGui::Begin("Framerate Counter", &Enabled_);

            // Set Initial Window Size
            ImGui::SetWindowSize(ImVec2(400,150), ImGuiCond_FirstUseEver);

            if (Visible) {

                // FPS Counter
                ImGui::Text("System Framerate %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            }

        // End System Info Window
        ImGui::End();
        

    }


}