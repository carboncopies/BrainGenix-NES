//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_RAMGraph.h>


GUI_Window_RAMGraph::GUI_Window_RAMGraph(ERS_STRUCT_SystemUtils* SystemUtils) {
    
    SystemUtils_ = SystemUtils;

}

GUI_Window_RAMGraph::~GUI_Window_RAMGraph() {

}

void GUI_Window_RAMGraph::Draw() {


    // Update Data In Vectors
    ERS_STRUCT_HardwareInfo HWInfo = SystemUtils_->ERS_HardwareInformation_->GetHWInfo();
    TotalRAM_.push_back(HWInfo.Dynamic_.PhysicalMemoryCapacity / 1073741824.0);
    FreeRAM_.push_back(HWInfo.Dynamic_.PhysicalMemoryFree / 1073741824.0);
    TotalSwap_.push_back(HWInfo.Dynamic_.SwapCapacity / 1073741824.0);
    FreeSwap_.push_back(HWInfo.Dynamic_.SwapFree / 1073741824.0);

    for (long i = 0; i < (long)FreeRAM_.size()-1000; i++) {
        FreeRAM_.erase(FreeRAM_.begin(), FreeRAM_.begin()+1);
    }

    // If Window Drawing Enabled
    if (Enabled_) {
        bool WindowVisible = ImGui::Begin("RAM Graph", &Enabled_);

            // Set Initial Window Size
            ImGui::SetWindowSize(ImVec2(300,250), ImGuiCond_FirstUseEver);

            // Check If Window Visible
            if (WindowVisible) {

                // Get Window Size (To Size Graph)
                ImVec2 WindowSize = ImGui::GetContentRegionAvail();
                ImVec2 GraphSize = ImVec2(WindowSize.x, WindowSize.y);

                // Graph
                if (WindowVisible && (SystemUtils_->FramerateManager_->ActualFrameTimesMS_.size() > 0)) {

                    ImPlot::SetNextAxesToFit();
                    bool PlotVisible = ImPlot::BeginPlot("RAM Information", GraphSize);


                    if (PlotVisible) {
                        //ImPlot::PlotLine("Total RAM (GB)", (const float*)TotalRAM_.data(), TotalRAM_.size());
                        ImPlot::PlotLine("Free RAM (GB)", (const float*)FreeRAM_.data(), FreeRAM_.size());
                        //ImPlot::PlotLine("Total Swap (GB)", (const float*)TotalSwap_.data(), TotalSwap_.size());
                        //ImPlot::PlotLine("Free Swap (GB)", (const float*)FreeSwap_.data(), FreeSwap_.size());


                        ImPlot::EndPlot();
                    }
                }

            }

        // End System Info Window
        ImGui::End();
        

    }


}