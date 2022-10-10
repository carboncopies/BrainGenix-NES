//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_ImportProgressBar.h>


GUI_Window_ImportProgressBar::GUI_Window_ImportProgressBar(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;

}

GUI_Window_ImportProgressBar::~GUI_Window_ImportProgressBar() {

}



void GUI_Window_ImportProgressBar::UpdateTotalItems(long Current, long Total) {

    CurrentAssetNumber_ = Current;
    TotalAssetsToImport_ = Total;

}

void GUI_Window_ImportProgressBar::UpdateJobState(bool JobFinished) {

    // Set Job State
    if (JobFinished) {
        ConsecFinished_ ++;
        IsJobFinishing_ = true;
    } else {
        ConsecFinished_ = 0;
        IsJobFinishing_ = false;
    }



}

void GUI_Window_ImportProgressBar::Draw() {


    if (Enabled_) {
    ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    bool Visible = ImGui::Begin("Import Status", &Enabled_, WindowFlags);

        // Set Window Size
        ImGui::SetWindowSize(ImVec2(300,0));

        // Set Window Position
        ImVec2 WindowSize = ImGui::GetWindowSize();
        ImVec2 WindowPos = ImVec2(SystemUtils_->RenderWidth_ - WindowSize.x, SystemUtils_->RenderHeight_ - WindowSize.y);
        ImGui::SetWindowPos(WindowPos);

        if (Visible) {

            // Calculate Stats
            if (!IsJobFinishing_) {
                if (TotalAssetsToImport_ == 0) {
                    PercentDone_ = 0.0f;
                } else {
                    PercentDone_ = (float)CurrentAssetNumber_ / (float)TotalAssetsToImport_;
                }
            } else {
                PercentDone_ = 1.0f;
            }

            // Draw Total Progres Bar
            ImGui::Text("Progress");
            ImGui::ProgressBar(PercentDone_);

        }

    ImGui::End();

    // Hide Window After Threshold Reached
    ConsecFinished_++;
    if (ConsecFinished_ >= ConsecFinishedThreshold_) {
        Enabled_ = false;
        ConsecFinished_ = 0;
    }


    }


}