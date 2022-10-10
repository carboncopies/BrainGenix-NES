//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_CLASS_SpotLightRenameModal.h>

Subwindow_SpotLightRenameModal::Subwindow_SpotLightRenameModal(ERS_CLASS_SceneManager* SceneManager) {

    SceneManager_ = SceneManager;


}

Subwindow_SpotLightRenameModal::~Subwindow_SpotLightRenameModal() {
    
}

void Subwindow_SpotLightRenameModal::Activate(int SceneIndex, int SpotLightIndex) {

    SelectedScene_ = SceneIndex;
    SelectedSpotLight_ = SpotLightIndex;
    Enabled_ = true;
    FirstFrame_ = true;        

}

void Subwindow_SpotLightRenameModal::Draw() {

    if (Enabled_) {
    ImGui::Begin("Rename Spot Light", &Enabled_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

        // Grab Window Focus
        ImGui::SetWindowFocus();
        
        // Set Default Keyboard Input
        if (FirstFrame_) {
            ImGui::SetKeyboardFocusHere(0);
            FirstFrame_ = false;
        }

        // Set Default Item To Type On, Add Input Box
        ImGui::SetItemDefaultFocus();
        ImGui::InputTextWithHint("Rename Spot Light", "Enter New Spot Light Name", SpotLightInputName_, IM_ARRAYSIZE(SpotLightInputName_));

        ImGui::Separator();

        // Rename And Cancel Buttons
        if (ImGui::Button("Rename", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ENTER)) { // If Button Pressed, Or Enter Key Pressed
            SceneManager_->Scenes_[SelectedScene_]->SpotLights[SelectedSpotLight_]->UserDefinedName = std::string(SpotLightInputName_);
            Enabled_ = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) { // If Button Pressed, Or Escape Key Pressed
            Enabled_ = false;
        }

    ImGui::End();
    }


}
