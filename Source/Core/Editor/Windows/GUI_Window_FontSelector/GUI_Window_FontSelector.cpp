//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//


#include <GUI_Window_FontSelector.h>


GUI_Window_FontSelector::GUI_Window_FontSelector(ERS_CLASS_FontManager* FontManager) {

    FontManager_ = FontManager;

}

GUI_Window_FontSelector::~GUI_Window_FontSelector() {

}


void GUI_Window_FontSelector::Draw() {

 if (Enabled_) {

        ImGuiWindowFlags Flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        if (ImGui::Begin("Font Selector", &Enabled_, Flags)) {
            ImGui::SetWindowSize(ImVec2(0,0));

                ImGui::BeginChild("Font Radio Buttons", ImVec2(300, 400), true);

                    for (int i = 0; (long)i < (long)FontManager_->FontNameList_.size(); i++) {
                        ImGui::RadioButton(FontManager_->FontNameList_[i].c_str(), &FontManager_->FontSelector_, i);
                    }

                ImGui::EndChild();

                ImGui::Separator();

                ImGui::SliderFloat("Font Size", &FontManager_->FontSize_, 5.0f, 30.0f);

                ImGui::Separator();

                if (ImGui::Button("Reload")) {
                    FontManager_->IndexFonts();
                }
                ImGui::SameLine();

                if (ImGui::Button("Apply")) {
                    FontManager_->UpdateFont_ = true;
                }
                ImGui::SameLine();

                if (ImGui::Button("Close")) {
                    Enabled_ = false;
                }


            ImGui::End();
        }

    }
}