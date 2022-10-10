//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_ObjectProperties.h>


GUI_Window_ObjectProperties::GUI_Window_ObjectProperties(Cursors3D* Cursors3D, ERS_CLASS_SceneManager* SceneManager, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_CLASS_VisualRenderer* VisualRenderer) {

    Cursors3D_ = Cursors3D;
    SceneManager_ = SceneManager;
    ProjectUtils_ = ProjectUtils;
    VisualRenderer_ = VisualRenderer;

    //ShaderNames_[0] = "Default";

}

GUI_Window_ObjectProperties::~GUI_Window_ObjectProperties() {
    
}

glm::vec3 GUI_Window_ObjectProperties::XYZDragFloat(std::string Name, glm::vec3 Input, float SnapAmount) {

    // Convert Input To Values
    float X, Y, Z;
    X = Input.x;
    Y = Input.y;
    Z = Input.z;

    // Format Table 
    ImGuiTableFlags TableFlags = ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedSame;
    if (ImGui::BeginTable(Name.c_str(), 4, TableFlags)) {

        ImGui::TableNextRow();

        // Draw Three Colored Boxes 
        ImGui::TableNextColumn();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
        ImGui::DragFloat("", &X, SnapAmount);
        ImGui::PopStyleColor();
        //ImGui::SameLine();

        ImGui::TableNextColumn();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.7f, 0.1f, 1.0f));
        ImGui::DragFloat("", &Y, SnapAmount);
        ImGui::PopStyleColor();
        //ImGui::SameLine();

        ImGui::TableNextColumn();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.1f, 0.7f, 1.0f));
        ImGui::DragFloat("", &Z, SnapAmount);
        ImGui::PopStyleColor();
        //ImGui::SameLine();

        // Draw Label
        ImGui::TableNextColumn();
        ImGui::Text("%s", Name.c_str());

    }
    ImGui::EndTable();

    // Return Value
    return glm::vec3(X, Y, Z);

}

void GUI_Window_ObjectProperties::Draw() {

    if (Enabled_) {
        bool Visible = ImGui::Begin("Object Properties", &Enabled_);

            // Set Initial Window Size
            ImGui::SetWindowSize(ImVec2(400,250), ImGuiCond_FirstUseEver);

            if (Visible && (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_.size() != 0)) {

                // LocRotScale Properties
                if (ImGui::CollapsingHeader("Physical Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
                    
                    // ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
                    float Colors[9] = {
                        0.7f, 0.1f, 0.1f,
                        0.1f, 0.7f, 0.1f,
                        0.1f, 0.1f, 0.7f
                    };

                    ImGui::DragFloat3("Location", (float*)glm::value_ptr(Cursors3D_->Pos_), 0.05f, Colors);
                    ImGui::DragFloat3("Rotation", (float*)glm::value_ptr(Cursors3D_->Rot_), 0.05f, Colors);
                    ImGui::DragFloat3("Scale", (float*)glm::value_ptr(Cursors3D_->Scale_), 0.05f, Colors, 0.0f, 65535.0f);
                }
            
                // Handle Extra Options For Lights
                unsigned long SelectedSceneObject = SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SelectedObject;
                if (SelectedSceneObject >= SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_.size()) {
                    SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SelectedObject = 0;
                    SelectedSceneObject = 0;
                }
                
                if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Type_ == std::string("PointLight")) {
                    
                    unsigned long Index = SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Index_;
                    if (ImGui::CollapsingHeader("Point Light Settings", ImGuiTreeNodeFlags_DefaultOpen)) {

                        float Color[3];
                        VecToFloat(&SceneManager_->Scenes_[SceneManager_->ActiveScene_]->PointLights[Index]->Color, Color);
                        ImGui::ColorEdit3("Color", Color);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Controls the main color of the light.");
                        FloatToVec(Color, &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->PointLights[Index]->Color);
  

                        ImGui::DragFloat("Intensity", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->PointLights[Index]->Intensity, 0.25f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the brightness of the light source, increase this for a brighter light.");

                        ImGui::DragFloat("MaxDistance", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->PointLights[Index]->MaxDistance, 0.5f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the distance after which the scene is no longer affected by this light source.");

                        ImGui::Checkbox("Cast Shadows", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->PointLights[Index]->CastsShadows_);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Allows or disallows this light from creating shadows on the scene.");


                    }

                } else if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Type_ == std::string("DirectionalLight")) {
                    
                    unsigned long Index = SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Index_;
                    if (ImGui::CollapsingHeader("Directional Light Settings", ImGuiTreeNodeFlags_DefaultOpen)) {

                        float Color[3];
                        VecToFloat(&SceneManager_->Scenes_[SceneManager_->ActiveScene_]->DirectionalLights[Index]->Color, Color);
                        ImGui::ColorEdit3("Color", Color);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Controls the main color of the light.");
                        FloatToVec(Color, &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->DirectionalLights[Index]->Color);


                        ImGui::DragFloat("Intensity", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->DirectionalLights[Index]->Intensity, 0.25f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the brightness of the light source, increase this for a brighter light.");

                        ImGui::DragFloat("MaxDistance", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->DirectionalLights[Index]->MaxDistance, 0.5f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the distance after which the scene is no longer affected by this light source.");

                        ImGui::Checkbox("Cast Shadows", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->DirectionalLights[Index]->CastsShadows_);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Allows or disallows this light from creating shadows on the scene.");

                    }

                } else if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Type_ == std::string("SpotLight")) {
                    
                    unsigned long Index = SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Index_;
                    if (ImGui::CollapsingHeader("Spot Light Settings", ImGuiTreeNodeFlags_DefaultOpen)) {

                        float Color[3];
                        VecToFloat(&SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->Color, Color);
                        ImGui::ColorEdit3("Color", Color);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Controls the main color of the light.");
                        FloatToVec(Color, &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->Color);


                        ImGui::DragFloat("Intensity", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->Intensity, 0.25f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the brightness of the light source, increase this for a brighter light.");

                        ImGui::DragFloat("MaxDistance", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->MaxDistance, 0.5f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the distance after which the scene is no longer affected by this light source.");


                        ImGui::DragFloat("Cutoff Angle", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->CutOff, 1.0f, 0.0f, 120.0f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Specifies the angle of the spot light's outer cone.");
                        ImGui::DragFloat("Rolloff Angle", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->Rolloff, 0.5f, 0.0f);
                        if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->CutOff < SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->Rolloff) {
                            SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->Rolloff = SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->CutOff;
                        } else if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->Rolloff < 0.0f) {
                            SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->Rolloff = 0.0f;
                        }
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the angle at which the outer cone begins to roll off. This angle sets the inner cone which is unaffected by rolloff. Rolloff occurs in the area between the outer and inner cone (this angle).");

                        ImGui::Checkbox("Cast Shadows", &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->CastsShadows_);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Allows or disallows this light from creating shadows on the scene.");

                    }

                } else if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Type_ == std::string("Model")) {
                    
                    unsigned long Index = SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Index_;
                    if (ImGui::CollapsingHeader("Model Settings", ImGuiTreeNodeFlags_DefaultOpen)) {

                        // Get Current Model
                        ERS_STRUCT_Model* Model = SceneManager_->Scenes_[SceneManager_->ActiveScene_]->Models[Index].get();

                        ImGui::Checkbox("Casts Dynamic Shadows", &Model->CastDynamicShadows_);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Indicates if this model will cast shadows in dynamic lights. Avoid using this whenever possible due to performance related issues.");

                        ImGui::Checkbox("Casts Static Shadows", &Model->CastStaticShadows_);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Indicates if this model will cast shadows in static lights.");

                        ImGui::Checkbox("Receive Shadows", &Model->ReceiveShadows_);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Allow this model to have shadows cast upon it by other objects as well as itself.");


                        ImGui::Checkbox("Treat Missing Textures As Transparent", &Model->TreatMissingTexturesAsTransparent_);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Primitive Solution to glass textures missing. ERS will replace missing diffuse textures with a transparent texture.");


                        ImGui::Checkbox("Render Model", &Model->Enabled);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Tell the rendering system to skip this model. Essentially makes it invisible.");


                        // Texture LOD Settings
                        ImGui::Separator();
                        ImGui::DragInt("Minimum LOD", &Model->UserLimitedMinLOD_, 1, 0, 10);
                        ImGui::DragInt("Maximum LOD", &Model->UserLimitedMaxLOD_, 1, 0, 10);
                        ImGui::Separator();


                        // Shader Override Settings
                        ImGui::Separator();

                        // Shader Control Menu
                        // int ShaderIndex = Model->ShaderOverrideIndex_ + 1;
                        // ImGui::Combo("Object Specific Shader", &ShaderIndex, );
                        // Model->ShaderOverrideIndex_ = ShaderIndex - 1;
                        
                        int ShaderIndex = Model->ShaderOverrideIndex_;
    


                        std::string PreviewValue;
                        if (ShaderIndex >= (int)VisualRenderer_->Shaders_.size()) {
                            PreviewValue = "Invalid Shader Index";

                        } else if (ShaderIndex == -1) {
                            PreviewValue = "Default Shader";
                        } else {
                            PreviewValue = VisualRenderer_->Shaders_[ShaderIndex]->DisplayName;
                        }

              


                        if (ImGui::BeginCombo("Object Specific Shader", PreviewValue.c_str())) {

                            if (ImGui::Selectable("Default Shader", ShaderIndex == -1)) {
                                Model->ShaderOverrideIndex_ = -1;
                            }

                            for (unsigned int i = 0; i < VisualRenderer_->Shaders_.size(); i++) {
                                if (ImGui::Selectable(VisualRenderer_->Shaders_[i]->DisplayName.c_str(), Model->ShaderOverrideIndex_ == i)) {
                                    Model->ShaderOverrideIndex_ = i;
                                }
                            }

                        ImGui::EndCombo();
                        }

                    }

                } else if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Type_ == std::string("SceneCamera")) {
                    
                    unsigned long Index = SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Index_;
                    if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen)) {

                        // Get Current Camera, Get Properties
                        ERS_STRUCT_SceneCamera* Camera = SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneCameras[Index].get();
                        

                        bool Selected = (bool)SceneManager_->Scenes_[SceneManager_->ActiveScene_]->ActiveSceneCameraIndex == Index;
                        if (ImGui::Checkbox("Active Camera", &Selected)) {
                            SceneManager_->Scenes_[SceneManager_->ActiveScene_]->ActiveSceneCameraIndex = Index;
                        }
                        ImGui::SameLine();
                        ImGui::HelpMarker("Indicates if this is the active camera or not. There can only be one active camera at a time. The system then renders the scene from the active camera's perspective on viewport 0.");


                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();



                        ImGui::DragFloat("FOV", &Camera->FOV_, 0.25f, 0.0f, 180.0f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the field of view (in degrees) of the camera");

                        ImGui::DragFloat("Near Clip Plane", &Camera->NearClip_, 0.25f, 0.0f, 10.0f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the minimum distance before which geometry is ignored.");

                        ImGui::DragFloat("Far Clip Plane", &Camera->FarClip_, 1.0f, 5.0f, 500.0f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the maximum distance after which geometry is ignored.");


                        ImGui::Spacing();


                        ImGui::SliderInt("Asset Streaming Priority", &Camera->StreamingPriority_, 0, 10);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Sets the priority of the camera on a scale from 1-10.");


                        ImGui::Spacing();


                        ImGui::Checkbox("Enforce Aspect Ratio", &Camera->EnforceAspectRatio_);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Manually override the camera's aspect ratio. Will cause letterboxing if the ratios don't match.");

                        if (!Camera->EnforceAspectRatio_) {
                            ImGui::BeginDisabled();
                        }
                        ImGui::DragFloat("Aspect Ratio", &Camera->AspectRatio_, 0.05f, 0.1f, 4.0f);
                        ImGui::SameLine();
                        ImGui::HelpMarker("Aspect ratio to override the camera's automatic one with. Ratio is width/height.");
                        if (!Camera->EnforceAspectRatio_) {
                            ImGui::EndDisabled();
                        }

                    }

                }


                // Show Script Data
                if (ImGui::CollapsingHeader("Scripts", ImGuiTreeNodeFlags_DefaultOpen)) {

                    // Populate Script List Char Array
                    long Index = SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Index_;
                    if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Type_ == std::string("Model")) {
                        ScriptIndices_ = &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->Models[Index]->AttachedScriptIndexes_;                  
                    } else if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Type_ == std::string("PointLight")) {
                        ScriptIndices_ = &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->PointLights[Index]->AttachedScriptIndexes_;     
                    } else if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Type_ == std::string("DirectionalLight")) {
                        ScriptIndices_ = &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->DirectionalLights[Index]->AttachedScriptIndexes_;     
                    } else if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Type_ == std::string("SpotLight")) {
                        ScriptIndices_ = &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SpotLights[Index]->AttachedScriptIndexes_;     
                    } else if (SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneObjects_[SelectedSceneObject].Type_ == std::string("SceneCamera")) {
                        ScriptIndices_ = &SceneManager_->Scenes_[SceneManager_->ActiveScene_]->SceneCameras[Index]->AttachedScriptIndexes_;     
                    }

                    // Draw List Box
                    if (ImGui::BeginChild("Script Controls", ImVec2(0, 200), true)) {


                        for (unsigned long i = 0; i < ScriptIndices_->size(); i++) {

                            unsigned int Index = (*ScriptIndices_)[i];


                            if (Index < ProjectUtils_->ProjectManager_->Project_.Scripts.size()) {

                                std::string ScriptName = ProjectUtils_->ProjectManager_->Project_.Scripts[Index].Name_;
                                bool Selected = i==(unsigned long)ScriptIndex_;
                                if (ImGui::Selectable(ScriptName.c_str(), &Selected)) {
                                    ScriptIndex_ = i;
                                }


                                // Context Menu
                                if (ImGui::BeginPopupContextItem()) {

                                    if (ImGui::MenuItem("Remove Script")) {
                                        ScriptIndices_->erase(ScriptIndices_->begin() + i);
                                    }


                                ImGui::EndPopup();
                                }

                            }

                        }


                    }
                    ImGui::EndChild();

                }

            }

        // End System Controls Window
        ImGui::End();

    }

}


void GUI_Window_ObjectProperties::VecToFloat(glm::vec3* In, float* Out) {

    Out[0] = In->x;
    Out[1] = In->y;
    Out[2] = In->z;

}

void GUI_Window_ObjectProperties::FloatToVec(float* In, glm::vec3* Out) {

    Out->x = In[0];
    Out->y = In[1];
    Out->z = In[2];

}