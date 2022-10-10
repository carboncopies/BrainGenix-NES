//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_ProjectSettings.h>


GUI_Window_ProjectSettings::GUI_Window_ProjectSettings(ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    ProjectUtils_ = ProjectUtils;
    SystemUtils_->Logger_->Log("Initializing ERS GUI Window Project Settings", 5);

}

GUI_Window_ProjectSettings::~GUI_Window_ProjectSettings() {

    SystemUtils_->Logger_->Log("ERS Window Project Settings Destructor Called", 6);

}


void GUI_Window_ProjectSettings::Draw() {

    if (Enabled_) {
    bool Visible = ImGui::Begin("Project Settings", &Enabled_);
    ImGui::SetWindowSize(ImVec2(725, 325), ImGuiCond_FirstUseEver);

        if (Visible) {
 
            // Copy Project Info Into Vars ImGui Can Understand
            strcpy(ProjectNameBuffer, ProjectUtils_->ProjectManager_->Project_.ProjectName.c_str());
            strcpy(ProjectCreationDateBuffer, ProjectUtils_->ProjectManager_->Project_.ProjectCreationDate.c_str());
            strcpy(ProjectModificationDateBuffer, ProjectUtils_->ProjectManager_->Project_.ProjectModificationDate.c_str());
            strcpy(ProjectDescriptionBuffer, ProjectUtils_->ProjectManager_->Project_.ProjectDescription.c_str());
            strcpy(ProjectLicenseNameBuffer, ProjectUtils_->ProjectManager_->Project_.ProjectLicense.c_str());
            strcpy(ProjectVersionBuffer, ProjectUtils_->ProjectManager_->Project_.ProjectVersion.c_str());
            IsProjectFree = !ProjectUtils_->ProjectManager_->Project_.IsLicenseProprietary;


            // Add Project Name, Description, Version
            ImGui::InputTextWithHint("Project Name", "Enter Project Title", ProjectNameBuffer, 512);
            ImGui::SameLine();
            ImGui::HelpMarker("Use this to set the title of your project. This is used to set the window title. (Up to 512 characters).");

            ImGui::InputTextMultiline("Project Description", ProjectDescriptionBuffer, 16384);
            ImGui::SameLine();
            ImGui::HelpMarker("Use this box to describe your project for other developers and users who enable the editor. (Up to 16384 characters).");

            ImGui::InputTextWithHint("Project Version", "Enter Project Version", ProjectVersionBuffer, 128);
            ImGui::SameLine();
            ImGui::HelpMarker("Set the version information about your project here. (Up to 128 characters).");


            // Set License Name/IsFree
            ImGui::Separator();
            ImGui::InputTextWithHint("Project License", "Enter License Name Here", ProjectLicenseNameBuffer, 128);
            ImGui::SameLine();
            ImGui::HelpMarker("Enter the name of the license that your project uses. Copyleft licenses are always better! (Up to 128 characters).");

            ImGui::Checkbox("Is License Free", &IsProjectFree);
            ImGui::SameLine();
            ImGui::HelpMarker("Checking this box indicates that your project and components are free and open source software.");

            // Check If Not Free, Present Warning
            if (!IsProjectFree) {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "WARNING: This project is not free and open source software!");
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "You may be subject to additional licensing restrictions and other nasty things.");
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Try contacting the developers to see if they will change the license.");            
            }
            ImGui::Separator();

            // Date Info
            ImGui::InputTextWithHint("Project Creation Date", "", ProjectCreationDateBuffer, 64, ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            ImGui::HelpMarker("The date when this project was created.");

            ImGui::InputTextWithHint("Project Modification Date", "", ProjectModificationDateBuffer, 64, ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            ImGui::HelpMarker("The date when this project was last modified.");


            // Populate Dropdown Menu
            ImGui::Separator();
            for (int i = 0; (long)i < (long)ProjectUtils_->SceneManager_->Scenes_.size(); i++) {
                ProjectScenes[i] = ProjectUtils_->SceneManager_->Scenes_[i]->SceneName.c_str();
            }
            ImGui::Combo("Default Scene", &ProjectUtils_->ProjectManager_->Project_.DefaultScene, ProjectScenes, ProjectUtils_->SceneManager_->Scenes_.size());
            ImGui::SameLine();
            ImGui::HelpMarker("Set what scene is opened when the project is opened both for editing and as the distributed version.");


            // Enable/Disbale Game Mode On Startup
            ImGui::Checkbox("Run On Startup", &ProjectUtils_->ProjectManager_->Project_.StartPlayingOnLoad);
            ImGui::SameLine();
            ImGui::HelpMarker("By default ERS will launch in editor mode, which disables scripts and shows the viewport camera. Checking this will automatically start scripts and viewport(s) will show game cameras instead.");


            // Copy In New Values
            ProjectUtils_->ProjectManager_->Project_.ProjectName = std::string(ProjectNameBuffer);
            ProjectUtils_->ProjectManager_->Project_.ProjectDescription = std::string(ProjectDescriptionBuffer);
            ProjectUtils_->ProjectManager_->Project_.ProjectLicense = std::string(ProjectLicenseNameBuffer);
            ProjectUtils_->ProjectManager_->Project_.IsLicenseProprietary = !IsProjectFree;
            ProjectUtils_->ProjectManager_->Project_.ProjectVersion = std::string(ProjectVersionBuffer);

        }


    ImGui::End();
    }

}