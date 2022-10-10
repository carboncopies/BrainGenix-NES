//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Menu_File.h>


GUI_Menu_File::GUI_Menu_File(ERS_STRUCT_SystemUtils* SystemUtils, ERS_CLASS_SceneManager* SceneManager, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_STRUCT_Windows* Windows) {

    SystemUtils_ = SystemUtils;
    SceneManager_ = SceneManager;
    ProjectUtils_ = ProjectUtils;
    Windows_ = Windows;

    SystemUtils_->Logger_->Log("Editor Setting Up File Menu", 4);

    SceneWriter_ = std::make_unique<SceneWriter>(SystemUtils_);


}

GUI_Menu_File::~GUI_Menu_File() {

    SystemUtils_->Logger_->Log("Editor Destroying File Menu", 4);

}

void GUI_Menu_File::Draw() {

    // File Menu
    if (ImGui::BeginMenu("File")) {

        ImGui::MenuItem("New", "", &Windows_->GUI_Window_NewProject_->Enabled_);
        ImGui::MenuItem("Open", "", &Windows_->GUI_Window_OpenProject_->Enabled_);
        ImGui::Separator();


        ImGui::Separator();
        if (ImGui::MenuItem("Save")) {

            SystemUtils_->Logger_->Log("Saving Project Data", 4);
            ProjectUtils_->ProjectManager_->WriteProject(1);

            SystemUtils_->Logger_->Log("Saving All Scenes", 4);
            for (int i = 0; (long)i < (long)SceneManager_->Scenes_.size(); i++) {
                SystemUtils_->Logger_->Log(std::string("Saving Data For Scene ") + std::to_string(i), 3);
                SceneWriter_->ProcessScene(
                    SceneManager_->Scenes_[i].get(),
                    SceneManager_->Scenes_[i]->ScenePath
                    );
            }
        }

        ImGui::MenuItem("Project Settings", "", &Windows_->GUI_Window_ProjectSettings_->Enabled_);
        ImGui::MenuItem("About", "", &Windows_->GUI_Window_About_->Enabled_);

        ImGui::Separator();

        ImGui::MenuItem("Import Model", "", &Windows_->GUI_Window_ImportModel_->Enabled_);
        ImGui::MenuItem("Import Models In Directory", "", &Windows_->GUI_Window_ImportModelDirectory_->Enabled_);

        ImGui::Separator();

        // Exit Options
        if (ImGui::MenuItem("Exit")) {
            *SystemUtils_->SystemShouldRun_ = false;
        }

        
    ImGui::EndMenu();
    }



}