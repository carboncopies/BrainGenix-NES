//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_RenderingSettings.h>

GUI_Window_RenderingSettings::GUI_Window_RenderingSettings(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Initializing Rendering Settings GUI_Window", 5);

    // Setup Default Values For Input Fields
    SystemUtils_->Logger_->Log("Setting Up Default Renderer Setting Input Fields", 3);

    SystemUtils_->Logger_->Log("Copying Shadow Map Resolution", 2);
    ERS_STRUCT_RendererSettings* Settings = SystemUtils_->RendererSettings_.get();
    DepthMapResolution_ = Settings->ShadowMapX_;


    if (Settings->ShadowFilteringType_ == ERS::Renderer::ERS_SHADOW_FILTERING_DISABLED) {
        SelectedShadowFiltering_ = 0;
    } else if (Settings->ShadowFilteringType_ == ERS::Renderer::ERS_SHADOW_FILTERING_PCF) {
        SelectedShadowFiltering_ = 1;
    } else if (Settings->ShadowFilteringType_ == ERS::Renderer::ERS_SHADOW_FILTERING_POISSON_SAMPLING) {
        SelectedShadowFiltering_ = 2;
    } else if (Settings->ShadowFilteringType_ == ERS::Renderer::ERS_SHADOW_FILTERING_STRATIFIED_POISSON_SAMPLING) {
        SelectedShadowFiltering_ = 3;
    }

    if (Settings->ShadowUpdateMode_ == ERS::Renderer::ERS_SHADOW_UPDATE_MODE_DISABLED) {
        SelectedShadowUpdates_ = 0;
    } else if (Settings->ShadowUpdateMode_ == ERS::Renderer::ERS_SHADOW_UPDATE_MODE_RANDOM) {
        SelectedShadowUpdates_ = 1;
    } else if (Settings->ShadowUpdateMode_ == ERS::Renderer::ERS_SHADOW_UPDATE_MODE_CONSECUTIVE) {
        SelectedShadowUpdates_ = 2;
    } else if (Settings->ShadowUpdateMode_ == ERS::Renderer::ERS_SHADOW_UPDATE_MODE_DISTANCE_PRIORITIZED) {
        SelectedShadowUpdates_ = 3;
    } else if (Settings->ShadowUpdateMode_ == ERS::Renderer::ERS_SHADOW_UPDATE_MODE_ALL) {
        SelectedShadowUpdates_ = 4;
    }


}

GUI_Window_RenderingSettings::~GUI_Window_RenderingSettings() {

}

void GUI_Window_RenderingSettings::Draw() {

    if (Enabled_) {
        bool Visible = ImGui::Begin("Rendering Settings", &Enabled_);

            // Set Initial Window Size
            ImGui::SetWindowSize(ImVec2(400,250), ImGuiCond_FirstUseEver);


            if (Visible) {

                // Handle Modifications To Renderer Settings
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                bool Apply = false;
                ERS_STRUCT_RendererSettings* Settings = SystemUtils_->RendererSettings_.get();

                const char* ShadowFilteringNames[] = {
                    "ERS_SHADOW_FILTERING_DISABLED",
                    "ERS_SHADOW_FILTERING_PCF",
                    "ERS_SHADOW_FILTERING_POISSON_SAMPLING",
                    "ERS_SHADOW_FILTERING_STRATIFIED_POISSON_SAMPLING"
                };
                const char* ShadowUpdateNames[] = {
                    "ERS_SHADOW_UPDATE_MODE_DISABLED",
                    "ERS_SHADOW_UPDATE_MODE_RANDOM",
                    "ERS_SHADOW_UPDATE_MODE_CONSECUTIVE",
                    "ERS_SHADOW_UPDATE_MODE_DISTANCE_PRIORITIZED",
                    "ERS_SHADOW_UPDATE_MODE_ALL"
                };



                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Basic Settings");
                ImGui::Separator();

                // Wireframe Rendering Mode
                ImGui::Checkbox("Wireframe Rendering Mode", &OpenGLDrawLines_);
                ImGui::NewLine();

                // Rendering Background Clear Color
                ImGui::ColorEdit4("Background Clear Color", (float*)&ClearColor_);
                ImGui::NewLine();


                // Framerate Settings
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Framerate Settings");
                ImGui::Separator();

                // Framerate Cap (Linux Only, Windows Doesn't work for whatever reason)
                if (SystemUtils_->IsLinux_) {

                    int OldFrameRate = SystemUtils_->FramerateManager_->TargetFrameRate_;
                    ImGui::SliderInt("Target Framerate", &SystemUtils_->FramerateManager_->TargetFrameRate_, 10, 100);
                    if (OldFrameRate != SystemUtils_->FramerateManager_->TargetFrameRate_) {
                        SystemUtils_->FramerateManager_->SetTargetFramerate(SystemUtils_->FramerateManager_->TargetFrameRate_);
                    }
                }

                // Unlock FPS
                bool UnlockChanged = ImGui::Checkbox("Unlock Framerate", &SystemUtils_->FramerateManager_->UnlockFramerate_);

                // Sync To Monitor
                ImGui::SameLine();
                bool SyncChanged = ImGui::Checkbox("Sync To Monitor", &SystemUtils_->FramerateManager_->SyncToMonitor_);
   

                // Disallow Both To Be Checked At The Same Time
                if (!SystemUtils_->IsLinux_) {
                    if (UnlockChanged) {
                        if (SystemUtils_->FramerateManager_->UnlockFramerate_) {
                            SystemUtils_->FramerateManager_->SyncToMonitor_ = false;
                        } else {
                            SystemUtils_->FramerateManager_->SyncToMonitor_ = true;
                        }
                    } else if (SyncChanged) {
                        if (SystemUtils_->FramerateManager_->SyncToMonitor_) {
                            SystemUtils_->FramerateManager_->UnlockFramerate_ = false;
                        } else {
                            SystemUtils_->FramerateManager_->UnlockFramerate_ = true;
                        }
                    }
                } else {
                    if (UnlockChanged) {
                        if (SystemUtils_->FramerateManager_->UnlockFramerate_) {
                            SystemUtils_->FramerateManager_->SyncToMonitor_ = false;
                        }
                    } else if (SyncChanged) {
                        if (SystemUtils_->FramerateManager_->SyncToMonitor_) {
                            SystemUtils_->FramerateManager_->UnlockFramerate_ = false;
                        }
                    }
                }


                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Shadow Maps");
                ImGui::Separator();

                // Draw Buttons/Input Fields
                ImGui::InputInt("Depth Map Resolution", &DepthMapResolution_);
                
                ImGui::InputInt("Max Shadow Updates Per Frame", &Settings->MaxShadowUpdatesPerFrame_);
                ImGui::InputInt("Shadow Filter Kernel Size", &Settings->ShadowFilterKernelSize_);

                ImGui::Combo("Shadow Filtering", &SelectedShadowFiltering_, ShadowFilteringNames, 4);
                ImGui::Combo("Shadow Update Mode", &SelectedShadowUpdates_, ShadowUpdateNames, 5);
                
                // Handle Apply Button
                Apply = ImGui::Button("Apply");
                if (Apply) {
                    Settings->ShadowMapX_ = DepthMapResolution_;
                    Settings->ShadowMapY_ = DepthMapResolution_;

                    if (SelectedShadowFiltering_ == 0) {
                        Settings->ShadowFilteringType_ = ERS::Renderer::ERS_SHADOW_FILTERING_DISABLED;
                    } else if (SelectedShadowFiltering_ == 1) {
                        Settings->ShadowFilteringType_ = ERS::Renderer::ERS_SHADOW_FILTERING_PCF;
                    } else if (SelectedShadowFiltering_ == 2) {
                        Settings->ShadowFilteringType_ = ERS::Renderer::ERS_SHADOW_FILTERING_POISSON_SAMPLING;
                    } else if (SelectedShadowFiltering_ == 3) {
                        Settings->ShadowFilteringType_ = ERS::Renderer::ERS_SHADOW_FILTERING_STRATIFIED_POISSON_SAMPLING;
                    }

                    if (SelectedShadowUpdates_ == 0) {
                        Settings->ShadowUpdateMode_ = ERS::Renderer::ERS_SHADOW_UPDATE_MODE_DISABLED;
                    } else if (SelectedShadowUpdates_ == 1) {
                        Settings->ShadowUpdateMode_ = ERS::Renderer::ERS_SHADOW_UPDATE_MODE_RANDOM;
                    } else if (SelectedShadowUpdates_ == 2) {
                        Settings->ShadowUpdateMode_ = ERS::Renderer::ERS_SHADOW_UPDATE_MODE_CONSECUTIVE;
                    } else if (SelectedShadowUpdates_ == 3) {
                        Settings->ShadowUpdateMode_ = ERS::Renderer::ERS_SHADOW_UPDATE_MODE_DISTANCE_PRIORITIZED;
                    } else if (SelectedShadowUpdates_ == 4) {
                        Settings->ShadowUpdateMode_ = ERS::Renderer::ERS_SHADOW_UPDATE_MODE_ALL;
                    }

                }

            }



        // End System Controls Window
        ImGui::End();

    }

    // Update OpenGL Settings
    if (OpenGLDrawLines_) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glClearColor(ClearColor_.x, ClearColor_.y, ClearColor_.z, ClearColor_.w);


}