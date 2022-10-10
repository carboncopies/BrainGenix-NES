//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_CLASS_DirectionalLightRenameModal.h>

Subwindow_DirectionalLightRenameModal::Subwindow_DirectionalLightRenameModal(ERS_CLASS_SceneManager* SceneManager) {

    SceneManager_ = SceneManager;


}

Subwindow_DirectionalLightRenameModal::~Subwindow_DirectionalLightRenameModal() {
    
}

void Subwindow_DirectionalLightRenameModal::Activate(int SceneIndex, int DirectionalLightIndex) {

    SelectedScene_ = SceneIndex;
    SelectedDirectionalLight_ = DirectionalLightIndex;
    Enabled_ = true;
    FirstFrame_ = true;        

}

void Subwindow_DirectionalLightRenameModal::Draw() {

    if (Enabled_) {
    ImGui::Begin("Rename Directional Light", &Enabled_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

        // Grab Window Focus
        ImGui::SetWindowFocus();
        
        // Set Default Keyboard Input
        if (FirstFrame_) {
            ImGui::SetKeyboardFocusHere(0);
            FirstFrame_ = false;
        }

        // Set Default Item To Type On, Add Input Box
        ImGui::SetItemDefaultFocus();
        ImGui::InputTextWithHint("Rename Directional Light", "Enter New Directional Light Name", DirectionalLightInputName_, IM_ARRAYSIZE(DirectionalLightInputName_));

        ImGui::Separator();

        // Rename And Cancel Buttons
        if (ImGui::Button("Rename", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ENTER)) { // If Button Pressed, Or Enter Key Pressed
            SceneManager_->Scenes_[SelectedScene_]->DirectionalLights[SelectedDirectionalLight_]->UserDefinedName = std::string(DirectionalLightInputName_);
            Enabled_ = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) { // If Button Pressed, Or Escape Key Pressed
            Enabled_ = false;
        }

    ImGui::End();
    }


}
