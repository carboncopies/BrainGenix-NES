//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_AssetStreamingSettings.h>

GUI_Window_AssetStreamingSettings::GUI_Window_AssetStreamingSettings(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils) {

    SystemUtils_ = SystemUtils;
    ProjectUtils_ = ProjectUtils;
    ModelLoader_ = ProjectUtils_->ModelLoader_.get();
    SystemUtils_->Logger_->Log("Initializing AssetStreaming Settings GUI_Window", 5);

    // Copy In Default Parameters
    TextureStreamingThreads_ = ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->GetNumStreamerThreads();
    TextureLoadingThreads_ = ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->GetNumLoaderThreads();
    TextureStreamingQueueLimit_ = ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->GetQueueLimit();
    PreventDupeQueueEntries_ = ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->GetDupeQueueEntryPrevention();
    QueuePrioritizationEnabled_ = ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->GetQueuePrioritizationEnabled();

    VRAMBudgetMiB_ = SystemUtils_->RendererSettings_->VRAMBudget_ / 1048576;
    RAMBudgetMiB_  = SystemUtils_->RendererSettings_->RAMBudget_  / 1048576;
    VRAMWarningMiB_ = SystemUtils_->RendererSettings_->WarningLowVRAMBytes / 1048576;
    RAMWarningMiB_  = SystemUtils_->RendererSettings_->WarningLowRAMBytes  / 1048576;

    MaxThreads_ = std::thread::hardware_concurrency();

}

GUI_Window_AssetStreamingSettings::~GUI_Window_AssetStreamingSettings() {

}

void GUI_Window_AssetStreamingSettings::Draw() {

    if (Enabled_) {
        bool Visible = ImGui::Begin("Asset Streaming Settings", &Enabled_);

            // Set Initial Window Size
            ImGui::SetWindowSize(ImVec2(400,250), ImGuiCond_FirstUseEver);


            if (Visible) {

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Texture Streaming");
                ImGui::Separator();
                ImGui::Spacing();


                ImGui::DragFloat("VRAM Cutoff", &ModelLoader_->AssetStreamingManager_->DistanceCutoffVRAM_, 0.05f, 0.0f, 999.0f);
                ImGui::DragFloat("RAM Cutoff", &ModelLoader_->AssetStreamingManager_->DistanceCutoffRAM_, 0.05f, 0.0f, 999.0f);
                ImGui::Spacing();
                ImGui::Checkbox("Use Quadratic Scaling", &ModelLoader_->AssetStreamingManager_->UseQuadraticDistanceRolloff_);
                ImGui::DragFloat("Quadratic Component", &ModelLoader_->AssetStreamingManager_->QuadraticDistanceComponent_, 0.01f, -10.0f, 10.0f);
                ImGui::DragFloat("Linear Component", &ModelLoader_->AssetStreamingManager_->LinearDistanceComponent_, 0.02f, -20.0f, 20.0f);
                ImGui::DragFloat("Const Component", &ModelLoader_->AssetStreamingManager_->ConstantDistanceComponent_, 0.25f, -50.0f, 50.0f);
                ImGui::Spacing();
                ImGui::SliderInt("Minimum LOD", &ModelLoader_->AssetStreamingManager_->MinLOD_, 0, 99);
                ImGui::SliderInt("Maximum LOD", &ModelLoader_->AssetStreamingManager_->MaxLOD_, 0, 99);
                ImGui::InputInt("Max Texture Res", &ModelLoader_->AssetStreamingManager_->MaxTextureResolution_, 32, 128);
                

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::DragFloat("VRAM Budget (MiB)", &VRAMBudgetMiB_, 64.0f, 0.0f, 1048576);
                ImGui::DragFloat("RAM Budget (MiB)", &RAMBudgetMiB_, 128.0f, 0.0f, 536870912);

                ImGui::DragFloat("VRAM Warning Threshold (MiB)", &VRAMWarningMiB_, 64.0f, 0.0f, 32768.0f);
                ImGui::DragFloat("RAM Warning Threshold (MiB)", &RAMWarningMiB_, 64.0f, 0.0f, 65535.0f);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();


                ImGui::SliderInt("Texture Loading Threads", &TextureLoadingThreads_, 1, MaxThreads_);
                ImGui::SliderInt("Texture Streaming Threads", &TextureStreamingThreads_, 1, MaxThreads_);
                
                ImGui::SliderInt("Queue Limit", &TextureStreamingQueueLimit_, 1, MaxThreads_ * 25);
                ImGui::Checkbox("Prevent Duplicate Entry", &PreventDupeQueueEntries_);
                ImGui::Checkbox("Queue Prioritization", &QueuePrioritizationEnabled_);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                if (ImGui::Button("Apply")) {

                    // Update Queue Info
                    ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->SetQueueLimit(TextureStreamingQueueLimit_);
                    ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->SetDupeQueueEntryPrevention(PreventDupeQueueEntries_);
                    ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->SetQueuePrioritizationEnabled(QueuePrioritizationEnabled_);
                    
                    // Update Budget Info
                    SystemUtils_->RendererSettings_->VRAMBudget_ = VRAMBudgetMiB_ * 1048576;
                    SystemUtils_->RendererSettings_->RAMBudget_ = RAMBudgetMiB_ * 1048576;

                    SystemUtils_->RendererSettings_->WarningLowRAMBytes = RAMWarningMiB_ * 1048576;
                    SystemUtils_->RendererSettings_->WarningLowVRAMBytes = VRAMWarningMiB_ * 1048576;
                    
                    
                    // Update Threads
                    int LastLoaderThreadCount = ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->GetNumLoaderThreads();
                    if (LastLoaderThreadCount != TextureLoadingThreads_) {
                        ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->SetNumLoaderThreads(TextureLoadingThreads_);
                        ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->TeardownLoaderThreads();
                        ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->SetupLoaderThreads();
                    }

                    int LastStreamerThreadCount = ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->GetNumStreamerThreads();
                    if (LastStreamerThreadCount != TextureStreamingThreads_) {
                        ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->SetNumStreamerThreads(TextureStreamingThreads_);
                        ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->TeardownPusherThreads();
                        ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->SetupPusherThreads();
                    }
                }


                // Debug Buttons
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                if (ImGui::Button("Purge RAM Texture Cache")) {
                    ERS_STRUCT_Scene* ActiveScene = ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get();
                    for (unsigned int i = 0; i < ActiveScene->Models.size(); i++) {
                        ActiveScene->Models[i]->TargetTextureLevelRAM = 0;
                    }
                }
                if (ImGui::Button("Purge VRAM Texture Cache")) {
                    ERS_STRUCT_Scene* ActiveScene = ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get();
                    for (unsigned int i = 0; i < ActiveScene->Models.size(); i++) {
                        ActiveScene->Models[i]->TargetTextureLevelVRAM = 0;
                    }
                }
                if (ImGui::Button("Purge All Texture Cache")) {
                    ERS_STRUCT_Scene* ActiveScene = ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get();
                    for (unsigned int i = 0; i < ActiveScene->Models.size(); i++) {
                        ActiveScene->Models[i]->TargetTextureLevelVRAM = 0;
                        ActiveScene->Models[i]->TargetTextureLevelRAM = 0;
                    }
                }


            }



        // End System Controls Window
        ImGui::End();

    }

}