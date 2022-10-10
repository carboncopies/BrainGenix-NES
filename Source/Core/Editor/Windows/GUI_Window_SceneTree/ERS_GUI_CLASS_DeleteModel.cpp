//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_CLASS_DeleteModel.h>


Subwindow_DeleteModel::Subwindow_DeleteModel(ERS_CLASS_SceneManager* SceneManager, Cursors3D* Cursor) {

    SceneManager_ = SceneManager;
    Cursor_ = Cursor;


}

Subwindow_DeleteModel::~Subwindow_DeleteModel() {

}

void Subwindow_DeleteModel::DeleteModel(int SceneIndex, int ModelIndex) {

    FirstFrame_ = true;

    // Show Popup Windows
    SceneIndex_ = SceneIndex;
    ModelIndex_ = ModelIndex;
    ShowDeleteConfirm_ = true;
    
}

void Subwindow_DeleteModel::Draw() {
    UpdateConfirmDeletePopup();
}

void Subwindow_DeleteModel::UpdateConfirmDeletePopup() {

    if (ShowDeleteConfirm_) {
    ImGui::Begin("Delete Model?", &ShowDeleteConfirm_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

        // Grab Window Focus
        ImGui::SetWindowFocus();
        
        // Set Default Keyboard Input
        if (FirstFrame_) {
            ImGui::SetKeyboardFocusHere(0);
            FirstFrame_ = false;
        }

        // Add Confirm Message
        ImGui::SetItemDefaultFocus();
        ImGui::Text("This action will delete the selected model.");
        ImGui::Separator();

        // Confirm And Abort Buttons
        if (ImGui::Button("Confirm", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ENTER)) { // If Button Pressed, Or Enter Key Pressed
            SceneManager_->Scenes_[SceneIndex_]->Models.erase(SceneManager_->Scenes_[SceneIndex_]->Models.begin() + ModelIndex_);
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
