//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_CLASS_ModelRenameModal.h>

Subwindow_ModelRenameModal::Subwindow_ModelRenameModal(ERS_CLASS_SceneManager* SceneManager) {

    SceneManager_ = SceneManager;


}

Subwindow_ModelRenameModal::~Subwindow_ModelRenameModal() {
    
}

void Subwindow_ModelRenameModal::Activate(int SceneIndex, int ModelIndex) {

    SelectedScene_ = SceneIndex;
    SelectedModel_ = ModelIndex;
    Enabled_ = true;
    FirstFrame_ = true;        

}

void Subwindow_ModelRenameModal::Draw() {

    if (Enabled_) {
    ImGui::Begin("Rename Model", &Enabled_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

        // Grab Window Focus
        ImGui::SetWindowFocus();
        
        // Set Default Keyboard Input
        if (FirstFrame_) {
            ImGui::SetKeyboardFocusHere(0);
            FirstFrame_ = false;
        }

        // Set Default Item To Type On, Add Input Box
        ImGui::SetItemDefaultFocus();
        ImGui::InputTextWithHint("Rename Model", "Enter New Model Name", ModelInputName_, IM_ARRAYSIZE(ModelInputName_));

        ImGui::Separator();

        // Rename And Cancel Buttons
        if (ImGui::Button("Rename", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ENTER)) { // If Button Pressed, Or Enter Key Pressed
            SceneManager_->Scenes_[SelectedScene_]->Models[SelectedModel_]->Name = std::string(ModelInputName_);
            Enabled_ = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) { // If Button Pressed, Or Escape Key Pressed
            Enabled_ = false;
        }

    ImGui::End();
    }


}
