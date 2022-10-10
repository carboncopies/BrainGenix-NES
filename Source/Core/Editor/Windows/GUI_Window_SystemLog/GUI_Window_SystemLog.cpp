//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_SystemLog.h>


GUI_Window_SystemLog::GUI_Window_SystemLog(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Initializing GUI_Window_SystemLog", 5);

}

GUI_Window_SystemLog::~GUI_Window_SystemLog() {

    SystemUtils_->Logger_->Log("GUI_Window_SystemLog Destructor Called", 6);

}


void GUI_Window_SystemLog::Draw() {

    if (Enabled_) {
    bool Visible = ImGui::Begin("System Log", &Enabled_);

    ImGui::SetWindowSize(ImVec2(300,0), ImGuiCond_FirstUseEver);


        if (Visible) {

            // Autoscroll
            ImGui::Checkbox("Lock To Bottom", &AutoScroll_);
            ImGui::SameLine();


            // Clear Button
            if (ImGui::Button("Clear")) {
                StartingLogIndex_ = SystemUtils_->Logger_->LogLevels_.size();
            }        
            ImGui::SameLine();

            // Restore Button
            if (ImGui::Button("Restore")) {
                StartingLogIndex_ = 0;
            }
            ImGui::SameLine();

            // Min Log Level Slider
            ImGui::SliderInt("Minimum Log Level", &MinLogLevel_, 0, 10);

            ImGui::Separator();


            // Draw Log Textbox
            ImGui::BeginChild("Log Text", ImVec2(0,0), true, ImGuiWindowFlags_HorizontalScrollbar);
            for (int i = StartingLogIndex_; (unsigned int)i < SystemUtils_->Logger_->LogMessages_.size(); i++) {

                // Check Log Level
                if (SystemUtils_->Logger_->LogLevels_[i] > MinLogLevel_) {

                    float ColorRed = SystemUtils_->Logger_->LogColors_[i].Red / 255.0f;
                    float ColorGreen = SystemUtils_->Logger_->LogColors_[i].Green / 255.0f;
                    float ColorBlue = SystemUtils_->Logger_->LogColors_[i].Blue / 255.0f;


                    std::string LogText = SystemUtils_->Logger_->FullLogMessages_[i];
                    ImVec4 TextColor = ImVec4(ColorRed, ColorGreen, ColorBlue, 1.0f);
                    ImGui::TextColored(TextColor, "%s", LogText.c_str());

                }

            }
            // Scroll To Bottom If Enabled
            if (AutoScroll_) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();

        }


    ImGui::End();
    }


}