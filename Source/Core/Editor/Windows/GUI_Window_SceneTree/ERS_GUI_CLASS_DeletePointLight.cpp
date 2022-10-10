
//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_CLASS_DeletePointLight.h>


Subwindow_DeletePointLight::Subwindow_DeletePointLight(ERS_CLASS_SceneManager* SceneManager, Cursors3D* Cursor) {

    SceneManager_ = SceneManager;
    Cursor_ = Cursor;


}

Subwindow_DeletePointLight::~Subwindow_DeletePointLight() {

}

void Subwindow_DeletePointLight::DeletePointLight(int SceneIndex, int PointLightIndex) {

    FirstFrame_ = true;

    // Show Popup Windows
    SceneIndex_ = SceneIndex;
    PointLightIndex_ = PointLightIndex;
    ShowDeleteConfirm_ = true;
    
}

void Subwindow_DeletePointLight::Draw() {

    UpdateConfirmDeletePopup();

}

void Subwindow_DeletePointLight::UpdateConfirmDeletePopup() {

    if (ShowDeleteConfirm_) {
    ImGui::Begin("Delete Point Light?", &ShowDeleteConfirm_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

        // Grab Window Focus
        ImGui::SetWindowFocus();
        
        // Set Default Keyboard Input
        if (FirstFrame_) {
            ImGui::SetKeyboardFocusHere(0);
            FirstFrame_ = false;
        }

        // Add Confirm Message
        ImGui::SetItemDefaultFocus();
        ImGui::Text("This action will delete the selected Point Light.");
        ImGui::Separator();

        // Confirm And Abort Buttons
        if (ImGui::Button("Confirm", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ENTER)) { // If Button Pressed, Or Enter Key Pressed
            SceneManager_->Scenes_[SceneIndex_]->PointLights.erase(SceneManager_->Scenes_[SceneIndex_]->PointLights.begin() + PointLightIndex_);
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
