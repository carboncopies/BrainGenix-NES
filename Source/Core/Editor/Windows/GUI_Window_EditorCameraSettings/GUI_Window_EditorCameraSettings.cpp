//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_EditorCameraSettings.h>

GUI_Window_EditorCameraSettings::GUI_Window_EditorCameraSettings(ERS_STRUCT_SystemUtils* SystemUtils, ERS_CLASS_VisualRenderer* VisualRenderer) {

    SystemUtils_    = SystemUtils;
    VisualRenderer_ = VisualRenderer;
    SystemUtils_->Logger_->Log("Initializing Editor Camera Settings GUI_Window", 5);


}

GUI_Window_EditorCameraSettings::~GUI_Window_EditorCameraSettings() {

}

void GUI_Window_EditorCameraSettings::Draw() {

    if (Enabled_) {
        bool Visible = ImGui::Begin("Editor Camera Settings", &Enabled_);

        // Set Initial Window Size
        ImGui::SetWindowSize(ImVec2(460,330), ImGuiCond_FirstUseEver);

        // Limit Index
        SelectedViewportIndex_ = std::min((int)VisualRenderer_->Viewports_.size() - 1, SelectedViewportIndex_);


        if (Visible) {

            // Show Information About Every Viewport
            if (ImGui::BeginCombo("Viewport", VisualRenderer_->Viewports_[SelectedViewportIndex_]->Name.c_str())) {
                for (unsigned int i = 0; i < VisualRenderer_->Viewports_.size(); i++) {
                    bool IsSelected = SelectedViewportIndex_ == (int)i;
                    if (ImGui::Selectable(VisualRenderer_->Viewports_[i]->Name.c_str(), &IsSelected)) {
                        SelectedViewportIndex_ = i;
                    }
                }
            ImGui::EndCombo();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();


            ERS_STRUCT_Viewport* Viewport =  VisualRenderer_->Viewports_[SelectedViewportIndex_].get();     


            // Movement Speed Controls
            if (ImGui::CollapsingHeader("Movement Speed", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Spacing();

                float MinSpeed, MaxSpeed, CurrentSpeed;
                Viewport->Processor->GetMovementSpeedBoundries(MinSpeed, MaxSpeed);
                Viewport->Processor->GetMovementSpeed(CurrentSpeed);
                ImGui::DragFloat("Movement Speed", &CurrentSpeed, 0.025f, 0.0f, 100.0f);
                ImGui::DragFloat("Minimum Speed", &MinSpeed, 0.05f, 0.0f, 10.0f);
                ImGui::DragFloat("Maximum Speed", &MaxSpeed, 0.05f, 0.0f, 100.0f);
                Viewport->Processor->SetMovementSpeed(CurrentSpeed);
                Viewport->Processor->SetMovementSpeedBoundries(MinSpeed, MaxSpeed);
                
                ImGui::Spacing();
            }

            // Mouse Controls
            if (ImGui::CollapsingHeader("Mouse Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Spacing();

                bool ConstrainPitch;
                float MouseSensitivity;
                Viewport->Processor->GetPitchConstraint(ConstrainPitch);
                Viewport->Processor->GetMouseSensitivity(MouseSensitivity);
                ImGui::Checkbox("Constrain Pitch", &ConstrainPitch);
                ImGui::DragFloat("Mouse Sensitivity", &MouseSensitivity, 0.0025f, 0.001f, 1.0f);
                Viewport->Processor->SetPitchConstraint(ConstrainPitch);
                Viewport->Processor->SetMouseSensitivity(MouseSensitivity);

                ImGui::Spacing();
            }

            // Location/Rotation Information
            if (ImGui::CollapsingHeader("LocRot Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Spacing();

                float Colors[9] = {
                    0.7f, 0.1f, 0.1f,
                    0.1f, 0.7f, 0.1f,
                    0.1f, 0.1f, 0.7f
                };

                glm::vec3 Pos, Rot;
                Viewport->Processor->GetPosition(Pos);
                Viewport->Processor->GetRotation(Rot);
                ImGui::DragFloat3("Location", (float*)glm::value_ptr(Pos), 0.05f, Colors);
                ImGui::DragFloat3("Rotation", (float*)glm::value_ptr(Rot), 0.05f, Colors);
                Viewport->Processor->SetPosition(Pos);
                Viewport->Processor->SetRotation(Rot);

                ImGui::Spacing();
            }

            // Camera Information
            if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Spacing();

                float NearClip, FarClip, FOV;
                Viewport->Processor->GetClipBoundires(NearClip, FarClip);
                Viewport->Processor->GetFOV(FOV);
                ImGui::DragFloat("Near Clip", &NearClip, 0.1f, 0.0f, 10.0f);
                ImGui::DragFloat("Far Clip", &FarClip, 0.1f, 0.0f, 500.0f);
                ImGui::DragFloat("FOV", &FOV, 0.05f, 0.0f, 180.0f);
                Viewport->Processor->SetClipBoundries(NearClip, FarClip);
                Viewport->Processor->SetFOV(FOV);

                ImGui::Spacing();
            }


            Viewport->Processor->SetForceUpdate();

        }



    // End System Controls Window
    ImGui::End();

    }



}