//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_ImportModelDirectory.h>


GUI_Window_ImportModelDirectory::GUI_Window_ImportModelDirectory(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, GUI_Window_ImportProgressBar* GUI_Window_ImportProgressBar) {

    SystemUtils_ = SystemUtils;
    ProjectUtils_ = ProjectUtils;
    SystemUtils_->Logger_->Log("Initializing Asset Importer GUI", 5);

    GUI_Window_ImportProgressBar_ = GUI_Window_ImportProgressBar;

}


GUI_Window_ImportModelDirectory::~GUI_Window_ImportModelDirectory() {

    SystemUtils_->Logger_->Log("Asset Importer GUI Destructor Called", 6);

}

// Check String Endings (From: https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c)
bool EndsWith(const std::string& Input, const std::string& Ending) {
    if (Ending.size() > Input.size()) return false;
    return std::equal(Input.begin() + Input.size() - Ending.size(), Input.end(), Ending.begin());
}


inline void FileDialogCallback(const char *, void* vUserDatas, bool *) 
{
 
    ERS_STRUCT_ModelImportOptions* Options = (ERS_STRUCT_ModelImportOptions*)vUserDatas;


    ImGui::TextColored(ImVec4(0, 1, 1, 1), "Import Options");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Checkbox("Flip Textures", &Options->FlipTextures);

}



void GUI_Window_ImportModelDirectory::Draw() {

    if (Enabled_ && !AlreadyOpen_) {
        OpenFileDialog();
        AlreadyOpen_ = true;
    }

    // Draw File Dialog
    if (ImGuiFileDialog::Instance()->Display("Import Model Directory", ImGuiWindowFlags_None, ImVec2(800, 500))) {


        if (ImGuiFileDialog::Instance()->IsOk())
        {
            // Get List Of Files From Selection, Convert To Vector
            std::vector<std::string> FilePaths;
            std::vector<bool> FlipTextures;

            std::string Path = ImGuiFileDialog::Instance()->GetCurrentPath();
            ERS_STRUCT_ModelImportOptions* Options = (ERS_STRUCT_ModelImportOptions*)ImGuiFileDialog::Instance()->GetUserDatas();
            Options_ = *Options;

            Path += "/";

            for (const auto &Entry : std::filesystem::recursive_directory_iterator(Path)) {
                std::string FilePath{Entry.path().u8string()};
                if (EndsWith(FilePath, ".fbx") || EndsWith(FilePath, ".dae") || EndsWith(FilePath, ".obj") || EndsWith(FilePath, ".gltf") || EndsWith(FilePath, ".glb")) {
                    SystemUtils_->Logger_->Log(std::string("Adding Model '") + FilePath + "' To Import Queue", 5);
                    FilePaths.push_back(FilePath);
                    FlipTextures.push_back(Options_.FlipTextures);

                }

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

void GUI_Window_ImportModelDirectory::OpenFileDialog() {

    ImGuiFileDialog::Instance()->OpenDialog("Import Model Directory", "Import Model Directory", nullptr, "~", "", 
     std::bind(&FileDialogCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 350, 1, (void*)&Options_);


}