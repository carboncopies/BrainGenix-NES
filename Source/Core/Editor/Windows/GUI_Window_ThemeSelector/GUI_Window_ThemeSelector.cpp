//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//


#include <GUI_Window_ThemeSelector.h>


GUI_Window_ThemeSelector::GUI_Window_ThemeSelector(ERS_CLASS_ThemeManager* ThemeManager) {

    ThemeManager_ = ThemeManager;

}

GUI_Window_ThemeSelector::~GUI_Window_ThemeSelector() {

}


void GUI_Window_ThemeSelector::Draw() {

    if (Enabled_) {
    ImGuiWindowFlags Flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
    bool Visible = ImGui::Begin("Pick Color Theme", &Enabled_, Flags);

            ImGui::SetWindowSize(ImVec2(0, 0));


            if (Visible) {

                // Put Radio Buttons Here
                ImGui::BeginChild("Theme Selector", ImVec2(250, 250), true);

                    static int ThemeSelector = 0;
                    for (int i = 0; (long)i < (long)ThemeManager_->ThemeNames_.size(); i++) {

                        ImGui::RadioButton(ThemeManager_->ThemeNames_[i].c_str(), &ThemeSelector, i);

                    }
                    

                ImGui::EndChild();


                ImGui::Separator();


                // Reload Button
                if (ImGui::Button("Reload Themes")) {
                    ThemeManager_->LoadThemes();
                }
                ImGui::SameLine();

                // Apply Button
                if (ImGui::Button("Apply")) {
                    ThemeManager_->ApplyThemes(ThemeSelector);
                }
                ImGui::SameLine();

                // Close Button
                if (ImGui::Button("Close")) {
                    Enabled_ = false;
                }
            

            }

    ImGui::End();
    }

}