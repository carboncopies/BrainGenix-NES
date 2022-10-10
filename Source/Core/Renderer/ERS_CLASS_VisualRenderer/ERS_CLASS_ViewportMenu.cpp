//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_CLASS_ViewportMenu.h>


ERS_CLASS_ViewportMenu::ERS_CLASS_ViewportMenu(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, double* GameStartTime, bool* IsEditorMode, std::vector<std::unique_ptr<ERS_STRUCT_Shader>>* Shaders) {

    SystemUtils_ = SystemUtils;
    ProjectUtils_ = ProjectUtils;
    GameStartTime_ = GameStartTime;
    IsEditorMode_ = IsEditorMode;
    Shaders_ = Shaders;

    SystemUtils_->Logger_->Log("Initializing Viewport Menu Subsystem", 5);

}


ERS_CLASS_ViewportMenu::~ERS_CLASS_ViewportMenu() {

    SystemUtils_->Logger_->Log("Viewport Menu Subsystem Destructor Invoked", 6);

}




void ERS_CLASS_ViewportMenu::AddPointLight(ERS_CLASS_ShadowMaps* ShadowMaps) {

    std::shared_ptr<ERS_STRUCT_PointLight> Light = std::make_shared<ERS_STRUCT_PointLight>();
    ERS_STRUCT_Scene* Scene = ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get();

    Light->UserDefinedName = "New Point Light";
    Light->Color = glm::vec3(1.0f);

    Light->Intensity = 1.0f;
    Light->MaxDistance = 20.0f;


    Light->DepthMap.DepthMapTextureIndex = ShadowMaps->ERS_CLASS_DepthMaps_->AllocateDepthMapIndexCubemap();
    Light->DepthMap.Initialized = true;
    Light->DepthMap.ToBeUpdated = true;


    Scene->PointLights.push_back(Light);
    Scene->IndexSceneObjects();

}

void ERS_CLASS_ViewportMenu::AddDirectionalLight(ERS_CLASS_ShadowMaps* ShadowMaps) {

    std::shared_ptr<ERS_STRUCT_DirectionalLight> Light = std::make_shared<ERS_STRUCT_DirectionalLight>();
    ERS_STRUCT_Scene* Scene = ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get();

    Light->UserDefinedName = "New Directional Light";
    Light->Color = glm::vec3(1.0f);

    Light->Intensity = 5.0f;

    Light->MaxDistance = 100.0f;

    Light->DepthMap = ShadowMaps->ERS_CLASS_DepthMaps_->GenerateDepthMap2D();
    Light->DepthMap.Initialized = true;
    Light->DepthMap.ToBeUpdated = true;


    Scene->DirectionalLights.push_back(Light);
    Scene->IndexSceneObjects();

}

void ERS_CLASS_ViewportMenu::AddSpotLight(ERS_CLASS_ShadowMaps* ShadowMaps) {

    std::shared_ptr<ERS_STRUCT_SpotLight> Light = std::make_shared<ERS_STRUCT_SpotLight>();
    ERS_STRUCT_Scene* Scene = ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get();

    Light->UserDefinedName = "New Spot Light";
    Light->Color = glm::vec3(1.0f);

    
    Light->Intensity = 1.0f;
    Light->MaxDistance = 20.0f;

    Light->CutOff = 45.0f;
    Light->Rolloff = 10.0f;

    Light->DepthMap = ShadowMaps->ERS_CLASS_DepthMaps_->GenerateDepthMap2D();
    Light->DepthMap.Initialized = true;
    Light->DepthMap.ToBeUpdated = true;

    Scene->SpotLights.push_back(Light);
    Scene->IndexSceneObjects();

}

void ERS_CLASS_ViewportMenu::AddScene() {

    // Add Scene To List Of Scenes
    ERS_STRUCT_Scene NewScene;
    NewScene.ScenePath = SystemUtils_->ERS_IOSubsystem_->AllocateAssetID();
    NewScene.SceneName = "New Scene";
    NewScene.IsSceneLoaded = true;
    NewScene.SceneFormatVersion = 1;
    
    ProjectUtils_->ProjectManager_->Project_.SceneIDs.push_back(NewScene.ScenePath);

    ProjectUtils_->SceneManager_->AddScene(NewScene);


}

void ERS_CLASS_ViewportMenu::AddSceneCamera() {

    std::shared_ptr<ERS_STRUCT_SceneCamera> SceneCamera = std::make_shared<ERS_STRUCT_SceneCamera>();
    ERS_STRUCT_Scene* Scene = ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get();

    SceneCamera->UserDefinedName_ = "New Camera";

    Scene->SceneCameras.push_back(SceneCamera);
    Scene->IndexSceneObjects();

}


void ERS_CLASS_ViewportMenu::DrawMenu(ERS_STRUCT_Viewport* Viewport, ERS_CLASS_ShadowMaps* ShadowMaps) {


    // Menu Bar
    if (ImGui::BeginMenuBar()) {


        // Viewport Cube Controls
        if (ImGui::BeginMenu("Controls")) {

            // Draw Controls
            ImGui::MenuItem("Scene Info Overlay", nullptr, &Viewport->ShowSceneInfo_);
            ImGui::MenuItem("System Resources Overlay", nullptr, &Viewport->ShowResourceInfo_);
            ImGui::MenuItem("Loading Time Overlay", nullptr, &Viewport->ShowLoadingTimeInfo_);
            ImGui::MenuItem("Light Debug Overlay", nullptr, &Viewport->ShowLightInfo_);
            ImGui::MenuItem("Memory Info Overlay", nullptr, &Viewport->ShowMemoryInfo_);
            ImGui::MenuItem("RAM Loading Queue Overlay", nullptr, &Viewport->ShowRAMLoadingInfo_);
            ImGui::MenuItem("VRAM Loading Queue Overlay", nullptr, &Viewport->ShowVRAMLoadingInfo_);
            

            ImGui::Separator();

            ImGui::MenuItem("Rotation Indicator", nullptr, &Viewport->ShowCube);
            ImGui::MenuItem("Gizmo", nullptr, &Viewport->GizmoEnabled);
            ImGui::MenuItem("Grid", nullptr, &Viewport->GridEnabled);
            ImGui::MenuItem("Light Icons", nullptr, &Viewport->LightIcons);
            ImGui::MenuItem("Outline Selected Model", nullptr, &Viewport->ShowBoxOnSelectedModel_);
            ImGui::MenuItem("Model Bounding Boxes", nullptr, &Viewport->ShowBoundingBox_);

            ImGui::Separator();

            ImGui::MenuItem("Gamma Correction", nullptr, &Viewport->GammaCorrection);
            ImGui::MenuItem("HDR", nullptr, &Viewport->HDREnabled_);


        ImGui::EndMenu();
        }


        // Shader Control Menu
        if(ImGui::BeginMenu("Shader")) {

            // Draw Selectable Menu Showing Active Viewport Shader
            for (int i = 0; (long)i < (long)Shaders_->size(); i++) {

                if (strcmp((*Shaders_)[i]->DisplayName.substr(0, 1).c_str(), "_")) {
                    if (ImGui::Selectable((*Shaders_)[i]->DisplayName.c_str(), i == Viewport->ShaderIndex)) {
                        Viewport->ShaderIndex = i;
                    }
                }

            }

        ImGui::EndMenu();
        }

        // Grid Control Menu
        if (ImGui::BeginMenu("Grid")) {

            // Grid Scale Submenu
            if (ImGui::BeginMenu("Scale")) {

                if (ImGui::MenuItem("0.0625 Units", nullptr, (Viewport->Grid->GridSize_ == 0.00125f))) {
                    Viewport->Grid->GridSize_ = 0.00125f;
                }

                if (ImGui::MenuItem("0.125 Units", nullptr, (Viewport->Grid->GridSize_ == 0.0025f))) {
                    Viewport->Grid->GridSize_ = 0.0025f;
                }

                if (ImGui::MenuItem("0.25 Units", nullptr, (Viewport->Grid->GridSize_ == 0.005f))) {
                    Viewport->Grid->GridSize_ = 0.005f;
                }

                if (ImGui::MenuItem("0.5 Units", nullptr, (Viewport->Grid->GridSize_ == 0.01f))) {
                    Viewport->Grid->GridSize_ = 0.01f;
                }

                if (ImGui::MenuItem("1 Unit", nullptr, (Viewport->Grid->GridSize_ == 0.02f))) {
                    Viewport->Grid->GridSize_ = 0.02f;
                }

                if (ImGui::MenuItem("2.5 Units", nullptr, (Viewport->Grid->GridSize_ == 0.05f))) {
                    Viewport->Grid->GridSize_ = 0.05f;
                }

                if (ImGui::MenuItem("5 Units", nullptr, (Viewport->Grid->GridSize_ == 0.1f))) {
                    Viewport->Grid->GridSize_ = 0.1f;
                }
                
            ImGui::EndMenu();
            }

            // Line Thickness Submenu
            if (ImGui::BeginMenu("Line Thickness")) {

                if (ImGui::MenuItem("0.5%", nullptr, (Viewport->Grid->GridLineThickness_ == 0.005f))) {
                    Viewport->Grid->GridLineThickness_ = 0.005f;
                }

                if (ImGui::MenuItem("1%", nullptr, (Viewport->Grid->GridLineThickness_ == 0.01f))) {
                    Viewport->Grid->GridLineThickness_ = 0.01f;
                }

                if (ImGui::MenuItem("2%", nullptr, (Viewport->Grid->GridLineThickness_ == 0.02f))) {
                    Viewport->Grid->GridLineThickness_ = 0.02f;
                }

                if (ImGui::MenuItem("3%", nullptr, (Viewport->Grid->GridLineThickness_ == 0.03f))) {
                    Viewport->Grid->GridLineThickness_ = 0.03f;
                }

                if (ImGui::MenuItem("4%", nullptr, (Viewport->Grid->GridLineThickness_ == 0.04f))) {
                    Viewport->Grid->GridLineThickness_ = 0.04f;
                }
                
            ImGui::EndMenu();
            }

            // Color Scheme
            if (ImGui::BeginMenu("Colors")) {

                // Base Color
                if (ImGui::BeginMenu("Base")) {

                    if (ImGui::MenuItem("Red")) {
                        Viewport->Grid->GridColor_ = glm::vec3(1.0f, 0.0f, 0.0f);
                    }

                    if (ImGui::MenuItem("Green")) {
                        Viewport->Grid->GridColor_ = glm::vec3(0.0f, 1.0f, 0.0f);
                    }

                    if (ImGui::MenuItem("Blue")) {
                        Viewport->Grid->GridColor_ = glm::vec3(0.0f, 0.0f, 1.0f);
                    }

                    if (ImGui::MenuItem("White")) {
                        Viewport->Grid->GridColor_ = glm::vec3(1.0f);
                    }

                    if (ImGui::MenuItem("Light Grey")) {
                        Viewport->Grid->GridColor_ = glm::vec3(0.75f);
                    }

                    if (ImGui::MenuItem("Grey")) {
                        Viewport->Grid->GridColor_ = glm::vec3(0.5f);
                    }

                    if (ImGui::MenuItem("Dark Grey")) {
                        Viewport->Grid->GridColor_ = glm::vec3(0.25f);
                    }

                    if (ImGui::MenuItem("Very Dark Grey")) {
                        Viewport->Grid->GridColor_ = glm::vec3(0.1f);
                    }

                    if (ImGui::MenuItem("Black")) {
                        Viewport->Grid->GridColor_ = glm::vec3(0.0f);
                    }

                ImGui::EndMenu();
                }

                // X Axis Color
                if (ImGui::BeginMenu("X Axis")) {


                    if (ImGui::MenuItem("Red")) {
                        Viewport->Grid->GridColorX_ = glm::vec3(1.0f, 0.0f, 0.0f);
                    }

                    if (ImGui::MenuItem("Green")) {
                        Viewport->Grid->GridColorX_ = glm::vec3(0.0f, 1.0f, 0.0f);
                    }

                    if (ImGui::MenuItem("Blue")) {
                        Viewport->Grid->GridColorX_ = glm::vec3(0.0f, 0.0f, 1.0f);
                    }

                    if (ImGui::MenuItem("Dark Grey")) {
                        Viewport->Grid->GridColorX_ = glm::vec3(0.25f);
                    }

                    if (ImGui::MenuItem("Very Dark Grey")) {
                        Viewport->Grid->GridColorX_ = glm::vec3(0.1f);
                    }

                    if (ImGui::MenuItem("Black")) {
                        Viewport->Grid->GridColorX_ = glm::vec3(0.0f);
                    }

                ImGui::EndMenu();
                }


                // Z Axis Color
                if (ImGui::BeginMenu("Z Axis")) {


                    if (ImGui::MenuItem("Red")) {
                        Viewport->Grid->GridColorZ_ = glm::vec3(1.0f, 0.0f, 0.0f);
                    }

                    if (ImGui::MenuItem("Green")) {
                        Viewport->Grid->GridColorZ_ = glm::vec3(0.0f, 1.0f, 0.0f);
                    }

                    if (ImGui::MenuItem("Blue")) {
                        Viewport->Grid->GridColorZ_ = glm::vec3(0.0f, 0.0f, 1.0f);
                    }

                    if (ImGui::MenuItem("Dark Grey")) {
                        Viewport->Grid->GridColorZ_ = glm::vec3(0.25f);
                    }

                    if (ImGui::MenuItem("Very Dark Grey")) {
                        Viewport->Grid->GridColorZ_ = glm::vec3(0.1f);
                    }

                    if (ImGui::MenuItem("Black")) {
                        Viewport->Grid->GridColorZ_ = glm::vec3(0.0f);
                    }

                ImGui::EndMenu();
                }


            ImGui::EndMenu();
            }


        ImGui::EndMenu();
        }

        // Add Items Menu
        if (ImGui::BeginMenu("Add")) {

            if (ImGui::BeginMenu("Light")) {

                if (ImGui::MenuItem("Point Light")) {
                    AddPointLight(ShadowMaps);
                }

                if (ImGui::MenuItem("Spot Light")) {
                    AddSpotLight(ShadowMaps);
                }

                if (ImGui::MenuItem("Directional Light")) {
                    AddDirectionalLight(ShadowMaps);
                }

            ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Script")) {
                
                ERS_STRUCT_Script NewScript;
                NewScript.AssetID = SystemUtils_->ERS_IOSubsystem_->AllocateAssetID();
                NewScript.Code_ = "# ERS Script\n";
                NewScript.Name_ = "Untitled Script";
                ProjectUtils_->ProjectManager_->Project_.Scripts.push_back(NewScript);
            
            }

            if (ImGui::MenuItem("Scene")) {
                AddScene();
            }
            
            if (ImGui::MenuItem("Camera")) {
                AddSceneCamera();
            }

        ImGui::EndMenu();
        }

        // Grid Snapping Control Menu
        if (ImGui::BeginMenu("Grid Snapping")) {

            if (ImGui::BeginMenu("Translation")) {

                if (ImGui::MenuItem("Disabled", nullptr, (Viewport->GridSnapAmountTranslate_ == 0.0f))) {
                    Viewport->GridSnapAmountTranslate_ = 0.0f;
                }

                ImGui::Separator();
                
                if (ImGui::MenuItem("0.01275", nullptr, (Viewport->GridSnapAmountTranslate_ == 0.01275f))) {
                    Viewport->GridSnapAmountTranslate_ = 0.01275f;
                }
                if (ImGui::MenuItem("0.025", nullptr, (Viewport->GridSnapAmountTranslate_ == 0.025f))) {
                    Viewport->GridSnapAmountTranslate_ = 0.205f;
                }
                if (ImGui::MenuItem("0.05", nullptr, (Viewport->GridSnapAmountTranslate_ == 0.05f))) {
                    Viewport->GridSnapAmountTranslate_ = 0.05f;
                }
                if (ImGui::MenuItem("0.1", nullptr, (Viewport->GridSnapAmountTranslate_ == 0.1f))) {
                    Viewport->GridSnapAmountTranslate_ = 0.1f;
                }
                if (ImGui::MenuItem("0.25", nullptr, (Viewport->GridSnapAmountTranslate_ == 0.25f))) {
                    Viewport->GridSnapAmountTranslate_ = 0.25f;
                }
                if (ImGui::MenuItem("0.5", nullptr, (Viewport->GridSnapAmountTranslate_ == 0.5f))) {
                    Viewport->GridSnapAmountTranslate_ = 0.5f;
                }
                if (ImGui::MenuItem("0.75", nullptr, (Viewport->GridSnapAmountTranslate_ == 0.75f))) {
                    Viewport->GridSnapAmountTranslate_ = 0.75f;
                }
                if (ImGui::MenuItem("1.0", nullptr, (Viewport->GridSnapAmountTranslate_ == 1.0f))) {
                    Viewport->GridSnapAmountTranslate_ = 1.0f;
                }

            ImGui::EndMenu();
            }


            if (ImGui::BeginMenu("Rotate")) {

                if (ImGui::MenuItem("Disabled", nullptr, (Viewport->GridSnapAmountRotate_ == 0.0f))) {
                    Viewport->GridSnapAmountRotate_ = 0.0f;
                }

                ImGui::Separator();
                
                if (ImGui::MenuItem("1", nullptr, (Viewport->GridSnapAmountRotate_ == 1.0f))) {
                    Viewport->GridSnapAmountRotate_ = 1.0f;
                }
                if (ImGui::MenuItem("5", nullptr, (Viewport->GridSnapAmountRotate_ == 5.0f))) {
                    Viewport->GridSnapAmountRotate_ = 5.0f;
                }
                if (ImGui::MenuItem("10", nullptr, (Viewport->GridSnapAmountRotate_ == 10.0f))) {
                    Viewport->GridSnapAmountRotate_ = 10.0f;
                }
                if (ImGui::MenuItem("25", nullptr, (Viewport->GridSnapAmountRotate_ == 25.0f))) {
                    Viewport->GridSnapAmountRotate_ = 25.0f;
                }
                if (ImGui::MenuItem("45", nullptr, (Viewport->GridSnapAmountRotate_ == 45.0f))) {
                    Viewport->GridSnapAmountRotate_ = 45.0f;
                }
                if (ImGui::MenuItem("90", nullptr, (Viewport->GridSnapAmountRotate_ == 90.0f))) {
                    Viewport->GridSnapAmountRotate_ = 90.0f;
                }

            ImGui::EndMenu();
            }



            if (ImGui::BeginMenu("Scale")) {

                if (ImGui::MenuItem("Disabled", nullptr, (Viewport->GridSnapAmountScale_ == 0.0f))) {
                    Viewport->GridSnapAmountScale_ = 0.0f;
                }

                ImGui::Separator();
                
                if (ImGui::MenuItem("0.025", nullptr, (Viewport->GridSnapAmountScale_ == 0.025f))) {
                    Viewport->GridSnapAmountScale_ = 0.025f;
                }
                if (ImGui::MenuItem("0.05", nullptr, (Viewport->GridSnapAmountScale_ == 0.05f))) {
                    Viewport->GridSnapAmountScale_ = 0.05f;
                }
                if (ImGui::MenuItem("0.1", nullptr, (Viewport->GridSnapAmountScale_ == 0.1f))) {
                    Viewport->GridSnapAmountScale_ = 0.1f;
                }
                if (ImGui::MenuItem("0.25", nullptr, (Viewport->GridSnapAmountScale_ == 0.25f))) {
                    Viewport->GridSnapAmountScale_ = 0.25f;
                }
                if (ImGui::MenuItem("0.5", nullptr, (Viewport->GridSnapAmountScale_ == 0.5f))) {
                    Viewport->GridSnapAmountScale_ = 0.5f;
                }
                if (ImGui::MenuItem("0.75", nullptr, (Viewport->GridSnapAmountScale_ == 0.75f))) {
                    Viewport->GridSnapAmountScale_ = 0.75f;
                }
                if (ImGui::MenuItem("1.0", nullptr, (Viewport->GridSnapAmountScale_ == 1.0f))) {
                    Viewport->GridSnapAmountScale_ = 1.0f;
                }

            ImGui::EndMenu();
            }


        ImGui::EndMenu();
        }

        // Game Control Menu
        if (ImGui::BeginMenu("Run")) {

            // Run Option
            if (ImGui::MenuItem("Run With Editor", "F5")) {
                *IsEditorMode_ = false;
                *GameStartTime_ = glfwGetTime();
            }

            // Stop Option
            if (ImGui::MenuItem("Stop", "Escape")) {
                *IsEditorMode_ = !IsEditorMode_;
            }

        ImGui::EndMenu();
        }


        // Bounding Box Controls
        if (ImGui::BeginMenu("Bounding Box")) {

            // Options
            ImGui::MenuItem("Depth Test", nullptr, &Viewport->DisableBoundingBoxDepthTest_);
            ImGui::MenuItem("Wireframe", nullptr, &Viewport->WireframeBoundingBoxes_);

            // Mode selector
            if (ImGui::BeginMenu("Mode")) {
                if (ImGui::MenuItem("Plain Color", nullptr, Viewport->BoundingBoxRenderer->GetBoundingBoxDisplayMode() == 0)) {
                    Viewport->BoundingBoxRenderer->SetBoundingBoxDisplayMode(0);
                }

                if (ImGui::MenuItem("Target Texture Level (RAM)", nullptr, Viewport->BoundingBoxRenderer->GetBoundingBoxDisplayMode() == 1)) {
                    Viewport->BoundingBoxRenderer->SetBoundingBoxDisplayMode(1);
                }

                if (ImGui::MenuItem("Target Texture Level (VRAM)", nullptr, Viewport->BoundingBoxRenderer->GetBoundingBoxDisplayMode() == 2)) {
                    Viewport->BoundingBoxRenderer->SetBoundingBoxDisplayMode(2);
                }

                if (ImGui::MenuItem("Current Texture Level (RAM)", nullptr, Viewport->BoundingBoxRenderer->GetBoundingBoxDisplayMode() == 3)) {
                    Viewport->BoundingBoxRenderer->SetBoundingBoxDisplayMode(3);
                }

                if (ImGui::MenuItem("Current Texture Level (VRAM)", nullptr, Viewport->BoundingBoxRenderer->GetBoundingBoxDisplayMode() == 4)) {
                    Viewport->BoundingBoxRenderer->SetBoundingBoxDisplayMode(4);
                }
            ImGui::EndMenu();
            }

        ImGui::EndMenu();
        }

    ImGui::EndMenuBar();
    }


    // Keybinds
    if (ImGui::IsKeyPressed(GLFW_KEY_F5)) {
        *IsEditorMode_ = false;
        *GameStartTime_ = glfwGetTime();
    }
    if (ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) {
        *IsEditorMode_ = true;
    }


}