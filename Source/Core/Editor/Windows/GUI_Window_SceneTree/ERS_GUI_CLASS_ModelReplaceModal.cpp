//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_CLASS_ModelReplaceModal.h>

Subwindow_ModelReplaceModal::Subwindow_ModelReplaceModal(ERS_CLASS_SceneManager* SceneManager) {

    SceneManager_ = SceneManager;


}

Subwindow_ModelReplaceModal::~Subwindow_ModelReplaceModal() {
    
}

void Subwindow_ModelReplaceModal::Activate(int SceneIndex, int ModelIndex) {

    SelectedScene_ = SceneIndex;
    SelectedModel_ = ModelIndex;
    Enabled_ = true;
    FirstFrame_ = true;        

}

void Subwindow_ModelReplaceModal::Draw() {

    if (Enabled_) {
    ImGui::Begin("Replace Model", &Enabled_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);
  
        // Set Default Keyboard Input
        if (FirstFrame_) {
            ImGui::SetWindowFocus();
            ImGui::SetKeyboardFocusHere(0);
            ImGui::SetItemDefaultFocus();
            FirstFrame_ = false;
        }


        // Set Default Item To Type On, Add Input Box
        if (ImGui::BeginCombo("Replacement Model", SceneManager_->Scenes_[SelectedScene_]->Models[ReplacementModelIndex_]->Name.c_str())) {

            for (unsigned int i = 0; i < SceneManager_->Scenes_[SelectedScene_]->Models.size(); i++) {
                std::string ModelName = SceneManager_->Scenes_[SelectedScene_]->Models[i]->Name;
                bool ThisItemSelected = i==ReplacementModelIndex_;
                if (ImGui::Selectable(ModelName.c_str(), &ThisItemSelected)) {
                    ReplacementModelIndex_ = i;
                }

            }

        ImGui::EndCombo();
        }


        ImGui::Separator();

        // Replace And Cancel Buttons
        if (ImGui::Button("Replace", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ENTER)) { // If Button Pressed, Or Enter Key Pressed
            
            // Get ID of Model we're replacing as well as the info for the model to replace it with
            unsigned long IDBeingReplaced = SceneManager_->Scenes_[SelectedScene_]->Models[SelectedModel_]->AssetID;

            for (unsigned int i = 0; i < SceneManager_->Scenes_[SelectedScene_]->Models.size(); i++) {
                if (SceneManager_->Scenes_[SelectedScene_]->Models[i]->AssetID == (long)IDBeingReplaced) {

                    // Copy Loc/Rot/Scale/Scripts
                    glm::vec3 Pos, Rot, Scale;
                    std::vector<long> Scripts;
                    std::string Name;

                    Pos = SceneManager_->Scenes_[SelectedScene_]->Models[i]->ModelPosition;
                    Rot = SceneManager_->Scenes_[SelectedScene_]->Models[i]->ModelRotation;
                    Scale = SceneManager_->Scenes_[SelectedScene_]->Models[i]->ModelScale;
                    Scripts = SceneManager_->Scenes_[SelectedScene_]->Models[i]->AttachedScriptIndexes_;
                    Name = SceneManager_->Scenes_[SelectedScene_]->Models[i]->Name;

                    
                    // Overwrite With Copy Of Replacement Model
                    SceneManager_->Scenes_[SelectedScene_]->Models[i] = std::make_shared<ERS_STRUCT_Model>(*SceneManager_->Scenes_[SelectedScene_]->Models[ReplacementModelIndex_]);

                    // Copy Back Over Loc/Rot/Scale/Scripts
                    SceneManager_->Scenes_[SelectedScene_]->Models[i]->AttachedScriptIndexes_ = Scripts;
                    SceneManager_->Scenes_[SelectedScene_]->Models[i]->SetLocRotScale(Pos, Rot, Scale);
                    SceneManager_->Scenes_[SelectedScene_]->Models[i]->ApplyTransformations();
                    SceneManager_->Scenes_[SelectedScene_]->Models[i]->Name = Name;

                }
            }

            Enabled_ = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) { // If Button Pressed, Or Escape Key Pressed
            Enabled_ = false;
        }

    ImGui::End();
    }


}
