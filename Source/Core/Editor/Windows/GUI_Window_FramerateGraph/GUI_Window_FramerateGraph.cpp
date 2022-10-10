//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_FramerateGraph.h>


void GUI_Window_FramerateGraph::Draw() {

    // Push Back Current Framerate To Vector
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
        bool WindowVisible = ImGui::Begin("Framerate Graph", &Enabled_);

            // Set Initial Window Size
            ImGui::SetWindowSize(ImVec2(300,250), ImGuiCond_FirstUseEver);



            // Graph
            if (WindowVisible) {
                
                // Get Window Size (To Size Graph)
                ImVec2 WindowSize = ImGui::GetContentRegionAvail();
                ImVec2 GraphSize = ImVec2(WindowSize.x, WindowSize.y);

                ImPlot::SetNextAxesToFit();
                bool PlotVisible = ImPlot::BeginPlot("Framerate Graph", GraphSize);
                if (PlotVisible) {
                    ImPlot::PlotLine("Framerate (fps)", (const float*)FramerateHistory_.data(), FramerateHistory_.size());
                    ImPlot::EndPlot();
                }
            }

        // End System Info Window
        ImGui::End();
        

    }


}