//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_OpenProject.h>


GUI_Window_OpenProject::GUI_Window_OpenProject(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Seting Up Open Project Window Dialog", 5);

}


GUI_Window_OpenProject::~GUI_Window_OpenProject() {

    SystemUtils_->Logger_->Log("Open Project Window Dialog Destructor Called", 6);

}


void GUI_Window_OpenProject::Draw() {

    if (Enabled_ && !LastWindowState_) {
        ImGuiFileDialog::Instance()->OpenDialog("Open Project", "Open Project", nullptr, "~", "", 0);
    }

    if (Enabled_) {

        // Draw File Dialog
        if (ImGuiFileDialog::Instance()->Display("Open Project", ImGuiWindowFlags_None, ImVec2(800, 500))) {


            if (ImGuiFileDialog::Instance()->IsOk())
            {
               
                std::string Path = ImGuiFileDialog::Instance()->GetCurrentPath();
                Path += "/";
                SystemUtils_->Logger_->Log(std::string("Opening Project At Path '") + Path + "'", 5);
                
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
                    SystemUtils_->Logger_->Log("Shutting Down This Editor Window Now", 5);
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

