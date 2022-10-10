//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_NewProject.h>


GUI_Window_NewProject::GUI_Window_NewProject(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Seting Up New Project Window Dialog", 5);

}


GUI_Window_NewProject::~GUI_Window_NewProject() {

    SystemUtils_->Logger_->Log("New Project Window Dialog Destructor Called", 6);

}


void GUI_Window_NewProject::Draw() {

    if (Enabled_ && !LastWindowState_) {
        ImGuiFileDialog::Instance()->OpenDialog("New Project", "New Project", nullptr, "~", "", 0);

    }

    if (Enabled_) {

        // Draw File Dialog
        if (ImGuiFileDialog::Instance()->Display("New Project", ImGuiWindowFlags_None, ImVec2(800, 500))) {


            if (ImGuiFileDialog::Instance()->IsOk())
            {
               
                std::string Path = ImGuiFileDialog::Instance()->GetCurrentPath();
                Path += "/";
                SystemUtils_->Logger_->Log(std::string("Creating New Project In Target Directory '") + Path + "'", 5);
    
                // TodO add system to get project dir from config file,
                // then have it iterate over all files, copying them to the new selected path
                // finally, have the system load that
                // check for bugs and edge-cases

                std::string DefualtProjectPath = "EditorAssets/Projects/NewProject/";
                std::string CurrentExecutablePath = std::filesystem::current_path().u8string();

                for (const auto &Entry : std::filesystem::recursive_directory_iterator(DefualtProjectPath)) {

                    // Get The Current Absolute Path To File, As Well As It's Filename
                    std::string PathRelativeName{Entry.path().u8string()};
                    std::string File = CurrentExecutablePath + "/" + PathRelativeName;
                    std::string FileName = PathRelativeName.substr(PathRelativeName.find_last_of("/"), sizeof(PathRelativeName));

                    SystemUtils_->Logger_->Log(std::string("Copying File '") + File + "' To New Project Directory", 4);
                    std::filesystem::copy_file(File, Path + FileName);

                }


                std::string Command;
#if defined(_WIN32)
                Command += "start /B ";
#elif defined(__APPLE__)
                Command += "./";
#else
                Command += "./";
#endif
                Command += "BrainGenix-ERS -ProjectDirectory ";
                Command += '"' + Path + '"' + " &";
                int Status = std::system(Command.c_str());

                // Quit System
                if (Status != -1) {
                    SystemUtils_->Logger_->Log("Shutting Down This Editor Window Now, Launching Editor For That Project", 5);
                    *SystemUtils_->SystemShouldRun_ = false;
                } else {
                    SystemUtils_->Logger_->Log("Failed To Launch New ERS Instance!", 8);
                }

            }

        ImGuiFileDialog::Instance()->Close();
        }




    }

    LastWindowState_ = Enabled_;


}

