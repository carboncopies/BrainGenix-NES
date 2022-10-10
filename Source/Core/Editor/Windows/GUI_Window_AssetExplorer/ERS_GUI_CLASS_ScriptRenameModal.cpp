//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_GUI_CLASS_ScriptRenameModal.h>

Subwindow_ScriptRenameModal::Subwindow_ScriptRenameModal(ERS_STRUCT_ProjectUtils* ProjectUtils) {

    ProjectUtils_ = ProjectUtils;


}

Subwindow_ScriptRenameModal::~Subwindow_ScriptRenameModal() {
    
}

void Subwindow_ScriptRenameModal::Activate(int ScriptIndex) {

    SelectedScript_ = ScriptIndex;
    Enabled_ = true;
    FirstFrame_ = true;        

}

void Subwindow_ScriptRenameModal::Draw() {

    if (Enabled_) {
    ImGui::Begin("Rename Script", &Enabled_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

        // Grab Window Focus
        ImGui::SetWindowFocus();
        
        // Set Default Keyboard Input
        if (FirstFrame_) {
            ImGui::SetKeyboardFocusHere(0);
            FirstFrame_ = false;
        }

        // Set Default Item To Type On, Add Input Box
        ImGui::SetItemDefaultFocus();
        ImGui::InputTextWithHint("Rename Script", "Enter New Script Name", ScriptInputName_, IM_ARRAYSIZE(ScriptInputName_));

        ImGui::Separator();

        // Rename And Cancel Buttons
        if (ImGui::Button("Rename", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ENTER)) { // If Button Pressed, Or Enter Key Pressed
            ProjectUtils_->ProjectManager_->Project_.Scripts[SelectedScript_].Name_ = std::string(ScriptInputName_);
            Enabled_ = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) { // If Button Pressed, Or Escape Key Pressed
            Enabled_ = false;
        }

    ImGui::End();
    }


}
