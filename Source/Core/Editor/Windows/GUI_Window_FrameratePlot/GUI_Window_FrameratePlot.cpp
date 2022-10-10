//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_FrameratePlot.h>


void GUI_Window_FrameratePlot::Draw() {

    FramerateHistory_.push_back(ImGui::GetIO().Framerate);
    


    // Cap Array Length At Target Set
    if ((long)FramerateHistory_.size() > (long)HistoryLength_) {
        FramerateHistory_.erase(FramerateHistory_.begin());
    } else {

        // Fill With Zeros
        int Delta = HistoryLength_ - FramerateHistory_.size();
        for (int i = 0; i < Delta; i++) {
            FramerateHistory_.push_back(0.0f);
        }

    }

    if (Enabled_) {
        bool Visible = ImGui::Begin("Framerate Plot", &Enabled_);

            // Set Initial Window Size
            ImGui::SetWindowSize(ImVec2(300,250), ImGuiCond_FirstUseEver);


            if (Visible) {

                // Get Window Size (To Size Graph)
                ImVec2 WindowSize = ImGui::GetContentRegionAvail();
                ImVec2 GraphSize = ImVec2(WindowSize.x, WindowSize.y);

                // Plot
                ImGui::PlotLines("Framerate", (const float*)FramerateHistory_.data(), FramerateHistory_.size(), -1, NULL, -1.0f, 100.0f, GraphSize);

            }

        // End System Info Window
        ImGui::End();
        

    }


}