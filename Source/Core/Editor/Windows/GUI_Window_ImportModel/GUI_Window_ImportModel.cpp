//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_ImportModel.h>


GUI_Window_ImportModel::GUI_Window_ImportModel(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, GUI_Window_ImportProgressBar* GUI_Window_ImportProgressBar) {

    SystemUtils_ = SystemUtils;
    ProjectUtils_ = ProjectUtils;
    SystemUtils_->Logger_->Log("Initializing Asset Importer GUI", 5);

    GUI_Window_ImportProgressBar_ = GUI_Window_ImportProgressBar;

}


GUI_Window_ImportModel::~GUI_Window_ImportModel() {

    SystemUtils_->Logger_->Log("Asset Importer GUI Destructor Called", 6);

}


inline void FileDialogCallback(const char *, void* vUserDatas, bool *) 
{
 
    ERS_STRUCT_ModelImportOptions* Options = (ERS_STRUCT_ModelImportOptions*)vUserDatas;


    ImGui::TextColored(ImVec4(0, 1, 1, 1), "Import Options");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Checkbox("Flip Textures", &Options->FlipTextures);

}



void GUI_Window_ImportModel::Draw() {

    if (Enabled_ && !AlreadyOpen_) {
        OpenFileDialog();
        AlreadyOpen_ = true;
    }

    // Draw File Dialog
    if (ImGuiFileDialog::Instance()->Display("Import Model", ImGuiWindowFlags_None, ImVec2(800, 500))) {


        if (ImGuiFileDialog::Instance()->IsOk())
        {
            // Setup
            std::vector<std::string> FilePaths;
            std::vector<bool> FlipTextures;

            // Get Info  From Model Import
            std::map<std::string, std::string> selection = ImGuiFileDialog::Instance()->GetSelection(); // multiselection
            ERS_STRUCT_ModelImportOptions* Options = (ERS_STRUCT_ModelImportOptions*)ImGuiFileDialog::Instance()->GetUserDatas();
            Options_ = *Options;



            for (const auto& elem:selection) {
                FilePaths.push_back(elem.second);
                FlipTextures.push_back(Options_.FlipTextures);
            }

            // Add To Queue, Launch Import
            ProjectUtils_->ModelImporter_->AddToImportQueue(FilePaths, FlipTextures);
            GUI_Window_ImportProgressBar_->Enabled_ = true;
            Working_ = true;

        }

    ImGuiFileDialog::Instance()->Close();
    AlreadyOpen_ = false;
    Enabled_ = false;
    }


    // Update Window Stats
    if (GUI_Window_ImportProgressBar_->Enabled_ && Working_) {
        GUI_Window_ImportProgressBar_->UpdateTotalItems(ProjectUtils_->ModelImporter_->GetTotalItemsImported(), ProjectUtils_->ModelImporter_->GetTotalItemsToImport());

        Working_ = !ProjectUtils_->ModelImporter_->HasJobFinished();
        GUI_Window_ImportProgressBar_->UpdateJobState(!Working_);
        
    }


}

void GUI_Window_ImportModel::OpenFileDialog() {

    
    ImGuiFileDialog::Instance()->OpenDialog("Import Model", "Import Model", ".*", ".", "", 
        std::bind(&FileDialogCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 350, 1, (void*)&Options_);


}