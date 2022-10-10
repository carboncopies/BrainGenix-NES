//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_About.h>


GUI_Window_About::GUI_Window_About(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;

}

GUI_Window_About::~GUI_Window_About() {

}



void GUI_Window_About::Draw() {


    if (Enabled_) {
    bool Visible = ImGui::Begin("About", &Enabled_);

        // Set Window Size
        ImGui::SetWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);


        if (Visible) {
            
            // ERS Description
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "About:");
            ImGui::TextWrapped("BrainGenix Environment Rendering System (ERS) is a distributed multigpu "
                "rendering system with realtime physics, cloth and more. [Note: This project"
                " is currently in development].");

            // Operating System Status
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Feature Status:");
            if (!SystemUtils_->IsLinux_) {
                ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Some features are not available in Windows. To use all features, please use Linux/BSD.");
            } else {
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "All features are enabled. Congrats on using Linux/BSD, you're better off without Windows/OSX.");
            }

            // License Info
            ImGui::Separator();
            ImGui::Separator();
            ImGui::Separator();
            if (ImGui::CollapsingHeader("License")) {

                if (ImGui::BeginChild("InfoSubWindow")) {

                    ImGui::Separator();
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "License:");
                    ImGui::TextWrapped("This program is free software: you can redistribute it and/or modify "
                                "it under the terms of the GNU Affero General Public License as "
                                "published by the Free Software Foundation, either version 3 of the "
                                "License, or (at your option) any later version. "

                                "This program is distributed in the hope that it will be useful, "
                                "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                                "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
                                "GNU Affero General Public License for more details. "

                                "You should have received a copy of the GNU Affero General Public License "
                                "along with this program. If not, see <https://www.gnu.org/licenses/>.");

                ImGui::EndChild();
                }
            }

            // Write Text
            if (ImGui::CollapsingHeader("Info", ImGuiTreeNodeFlags_DefaultOpen)) {

                if (ImGui::BeginChild("InfoSubWindow")) {

                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Version:");
                    ImGui::SameLine();
                    ImGui::Text(ERS_VERSION);

                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Built On:");
                    ImGui::SameLine();
                    ImGui::Text(ERS_COMPILE_TIME_STAMP);

                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Target OS Name:");
                    ImGui::SameLine();
                    ImGui::Text(ERS_TARGET_OS_NAME);

                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Target OS Version:");
                    ImGui::SameLine();
                    ImGui::Text(ERS_TARGET_OS_VERSION);

                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Target CPU Architecture:");
                    ImGui::SameLine();
                    ImGui::Text(ERS_TARGET_PROCESSOR);               

                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Host OS Name:");
                    ImGui::SameLine();
                    ImGui::Text(ERS_HOST_OS_NAME);

                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Host OS Version:");
                    ImGui::SameLine();
                    ImGui::Text(ERS_HOST_OS_VERSION);

                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Host CPU Architecture:");
                    ImGui::SameLine();
                    ImGui::Text(ERS_HOST_PROCESSOR);    


                ImGui::EndChild();
                }

            }


        }

    ImGui::End();
    }


}