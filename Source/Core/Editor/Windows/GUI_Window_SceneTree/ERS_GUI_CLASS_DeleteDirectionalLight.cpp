
//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_CLASS_DeleteDirectionalLight.h>


Subwindow_DeleteDirectionalLight::Subwindow_DeleteDirectionalLight(ERS_CLASS_SceneManager* SceneManager, Cursors3D* Cursor) {

    SceneManager_ = SceneManager;
    Cursor_ = Cursor;


}

Subwindow_DeleteDirectionalLight::~Subwindow_DeleteDirectionalLight() {

}

void Subwindow_DeleteDirectionalLight::DeleteDirectionalLight(int SceneIndex, int DirectionalLightIndex) {

    FirstFrame_ = true;

    // Show Popup Windows
    SceneIndex_ = SceneIndex;
    DirectionalLightIndex_ = DirectionalLightIndex;
    ShowDeleteConfirm_ = true;
    
}

void Subwindow_DeleteDirectionalLight::Draw() {

    UpdateConfirmDeletePopup();

}

void Subwindow_DeleteDirectionalLight::UpdateConfirmDeletePopup() {

    if (ShowDeleteConfirm_) {
    ImGui::Begin("Delete Directional Light?", &ShowDeleteConfirm_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

        // Grab Window Focus
        ImGui::SetWindowFocus();
        
        // Set Default Keyboard Input
        if (FirstFrame_) {
            ImGui::SetKeyboardFocusHere(0);
            FirstFrame_ = false;
        }

        // Add Confirm Message
        ImGui::SetItemDefaultFocus();
        ImGui::Text("This action will delete the selected Directional Light.");
        ImGui::Separator();

        // Confirm And Abort Buttons
        if (ImGui::Button("Confirm", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ENTER)) { // If Button Pressed, Or Enter Key Pressed
            SceneManager_->Scenes_[SceneIndex_]->DirectionalLights.erase(SceneManager_->Scenes_[SceneIndex_]->DirectionalLights.begin() + DirectionalLightIndex_);
            ShowDeleteConfirm_ = false;
            SceneManager_->Scenes_[SceneManager_->ActiveScene_]->HasSelectionChanged = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Abort", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) { // If Button Pressed, Or Escape Key Pressed
            ShowDeleteConfirm_ = false;
        }

    ImGui::End();


    }

}
