//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_CLASS_PointLightRenameModal.h>

Subwindow_PointLightRenameModal::Subwindow_PointLightRenameModal(ERS_CLASS_SceneManager* SceneManager) {

    SceneManager_ = SceneManager;


}

Subwindow_PointLightRenameModal::~Subwindow_PointLightRenameModal() {
    
}

void Subwindow_PointLightRenameModal::Activate(int SceneIndex, int PointLightIndex) {

    SelectedScene_ = SceneIndex;
    SelectedPointLight_ = PointLightIndex;
    Enabled_ = true;
    FirstFrame_ = true;        

}

void Subwindow_PointLightRenameModal::Draw() {

    if (Enabled_) {
    ImGui::Begin("Rename Point Light", &Enabled_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

        // Grab Window Focus
        ImGui::SetWindowFocus();
        
        // Set Default Keyboard Input
        if (FirstFrame_) {
            ImGui::SetKeyboardFocusHere(0);
            FirstFrame_ = false;
        }

        // Set Default Item To Type On, Add Input Box
        ImGui::SetItemDefaultFocus();
        ImGui::InputTextWithHint("Rename Point Light", "Enter New Point Light Name", PointLightInputName_, IM_ARRAYSIZE(PointLightInputName_));

        ImGui::Separator();

        // Rename And Cancel Buttons
        if (ImGui::Button("Rename", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ENTER)) { // If Button Pressed, Or Enter Key Pressed
            SceneManager_->Scenes_[SelectedScene_]->PointLights[SelectedPointLight_]->UserDefinedName = std::string(PointLightInputName_);
            Enabled_ = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) { // If Button Pressed, Or Escape Key Pressed
            Enabled_ = false;
        }

    ImGui::End();
    }


}
