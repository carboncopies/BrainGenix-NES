//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_SceneTree.h>

GUI_Window_SceneTree::GUI_Window_SceneTree(ERS_CLASS_SceneManager* SceneManager, ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, Cursors3D* Cursors3D) {

    SceneManager_ = SceneManager;
    SystemUtils_ = SystemUtils;
    ProjectUtils_ = ProjectUtils;
    Cursors3D_ = Cursors3D;
    SystemUtils_->Logger_->Log("Initializing ERS GUI Window_SceneTree", 4);

    Subwindow_SceneRenameModal_ = std::make_unique<Subwindow_SceneRenameModal>(SceneManager_);
    Subwindow_ModelRenameModal_ = std::make_unique<Subwindow_ModelRenameModal>(SceneManager_);
    Subwindow_PointLightRenameModal_ = std::make_unique<Subwindow_PointLightRenameModal>(SceneManager_);
    Subwindow_DirectionalLightRenameModal_ = std::make_unique<Subwindow_DirectionalLightRenameModal>(SceneManager_);
    Subwindow_SpotLightRenameModal_ = std::make_unique<Subwindow_SpotLightRenameModal>(SceneManager_);

    Subwindow_ModelReplaceModal_ = std::make_unique<Subwindow_ModelReplaceModal>(SceneManager_);
    
    Subwindow_DeleteScene_ = std::make_unique<Subwindow_DeleteScene>(SceneManager_, Cursors3D_);
    Subwindow_DeleteModel_ = std::make_unique<Subwindow_DeleteModel>(SceneManager_, Cursors3D_);
    Subwindow_DeletePointLight_ = std::make_unique<Subwindow_DeletePointLight>(SceneManager_, Cursors3D_);
    Subwindow_DeleteDirectionalLight_ = std::make_unique<Subwindow_DeleteDirectionalLight>(SceneManager_, Cursors3D_);
    Subwindow_DeleteSpotLight_ = std::make_unique<Subwindow_DeleteSpotLight>(SceneManager_, Cursors3D_);

    SystemUtils_->Logger_->Log("Finished Initializing ERS GUI Window_SceneTree", 5);

}

GUI_Window_SceneTree::~GUI_Window_SceneTree() {


}

void GUI_Window_SceneTree::Draw() {


    if (Enabled_) {
    bool Visible = ImGui::Begin("Scene Tree", &Enabled_);

        // Set Initial Window Size
        ImGui::SetWindowSize(ImVec2(400,250), ImGuiCond_FirstUseEver);


        if (Visible) {

            // Initialize Vars
            int ActiveScene = SceneManager_->ActiveScene_;
            
            // Convert Vector to Array
            if (ImGui::BeginCombo("Active Scene", SceneManager_->Scenes_[ActiveScene]->SceneName.c_str())) {

                for (int i = 0; (long)i < (long)SceneManager_->Scenes_.size(); i++) {

                    // Setup Selector
                    bool Selector = false;
                    if (i == SceneManager_->ActiveScene_) {
                        Selector = true;
                    }

                    // Create Selectable Item
                    ImGui::Selectable(SceneManager_->Scenes_[i]->SceneName.c_str(), &Selector);

                    // If Item Selected, Update Scene To Current Index
                    if (Selector) {
                        SceneManager_->ActiveScene_ = i;
                        SceneManager_->Scenes_[i]->HasSelectionChanged = true;
                        SceneManager_->Scenes_[i]->IndexSceneObjects();
                    }
                }

            ImGui::EndCombo();
            }

            // Active Scene Dropdown
            ImGui::Separator();




            // Draw Selector In Child Frame
            if (ImGui::BeginChild("Tree Selector")) {


                // Create Scene Trees
                for (int SceneIndex = 0; (long)SceneIndex < (long)SceneManager_->Scenes_.size(); SceneIndex++) {

                    // Setup Tree Flags
                    ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
                    if (SceneIndex == ActiveScene) {
                        NodeFlags |= ImGuiTreeNodeFlags_Selected;
                        NodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
                    }

                    // Get Tree Metadata
                    const char* SceneName = SceneManager_->Scenes_[SceneIndex]->SceneName.c_str();
                    //const char* PopupName = std::string(std::string("SceneTreePopupMenu_") + std::to_string(SceneIndex)).c_str();


                    // Begin Tree
                    bool TreeNode = ImGui::TreeNodeEx((void*)(intptr_t)SceneIndex, NodeFlags, "%s", SceneName);

                    // Context Menu
                    if (ImGui::BeginPopupContextItem()) {

                        // Menu Items
                        if (ImGui::MenuItem("Rename")) {
                            Subwindow_SceneRenameModal_->Activate(SceneIndex);
                        }
                        if (ImGui::MenuItem("Duplicate")) {
                            GUI_Windowutil_DuplicateScene(SceneManager_, SceneIndex); // FIXME: Will need to update how scenes are saved, as right now these will overwrite other scenes when saved. (Solution could be a scenes folder?)
                        }



                        ImGui::Separator();

                        if (ImGui::MenuItem("Delete")) {
                            Subwindow_DeleteScene_->DeleteScene(SceneIndex);
                        }


                    ImGui::EndPopup();
                    }



                    if (TreeNode) {

                        DrawScene(SceneManager_->Scenes_[SceneIndex].get(), SceneIndex);

                        ImGui::TreePop();
                    }
                }
            }



        ImGui::EndChild();
        }
        


        // Drag/Drop Target
        long PayloadID;
        if (ImGui::BeginDragDropTarget()) {

            if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("PAYLOAD_ASSET_MODEL_ID")) {
                memcpy(&PayloadID, Payload->Data, sizeof(long));
                SystemUtils_->Logger_->Log(std::string(std::string("Window_SceneTree Recieved Drag Drop Payload 'PAYLOAD_ASSET_MODEL_ID' With Value '") + std::to_string(PayloadID) + std::string("'")).c_str(), 0);
                ERS_STRUCT_Scene* Scene = ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get();
                ProjectUtils_->SceneLoader_->AddModel(Scene, PayloadID);
            }

        ImGui::EndDragDropTarget();
        }





    // End System Controls Window
    ImGui::End();
    }


    // Draw Popup Modals
    Subwindow_SceneRenameModal_->Draw();
    Subwindow_ModelRenameModal_->Draw();
    Subwindow_PointLightRenameModal_->Draw();
    Subwindow_DirectionalLightRenameModal_->Draw();
    Subwindow_SpotLightRenameModal_->Draw();
    Subwindow_DeleteScene_->Draw();
    Subwindow_DeleteModel_->Draw();
    Subwindow_DeletePointLight_->Draw();
    Subwindow_DeleteDirectionalLight_->Draw();
    Subwindow_DeleteSpotLight_->Draw();
    Subwindow_ModelReplaceModal_->Draw();

}


void GUI_Window_SceneTree::DrawScene(ERS_STRUCT_Scene* Scene, int SceneIndex) {

    // Get Selected Item
    int SelectedSceneObjectIndex = Scene->SelectedObject;


    // Sort Objects
    std::vector<ERS_STRUCT_SceneObject> Models;
    std::vector<unsigned long> ModelIndexes; // stores the index of the associated element in the Models list from the sceneobjects list
    std::vector<ERS_STRUCT_SceneObject> PointLights;
    std::vector<unsigned long> PointLightIndexes; // stores the index of the associated element in the Models list from the sceneobjects list
    std::vector<ERS_STRUCT_SceneObject> DirectionalLights;
    std::vector<unsigned long> DirectionalLightIndexes; // stores the index of the associated element in the Models list from the sceneobjects list
    std::vector<ERS_STRUCT_SceneObject> SpotLights;
    std::vector<unsigned long> SpotLightIndexes; // stores the index of the associated element in the Models list from the sceneobjects list
    std::vector<ERS_STRUCT_SceneObject> SceneCameras;
    std::vector<unsigned long> SceneCameraIndexes; // stores the index of the associated element in the SceneCamera list


    for (unsigned long i = 0; i < Scene->SceneObjects_.size(); i++) {

        if (Scene->SceneObjects_[i].Type_ == "Model") {
            Models.push_back(Scene->SceneObjects_[i]);
            ModelIndexes.push_back(i);
        } else if (Scene->SceneObjects_[i].Type_ == "PointLight") {
            PointLights.push_back(Scene->SceneObjects_[i]);
            PointLightIndexes.push_back(i);
        } else if (Scene->SceneObjects_[i].Type_ == "DirectionalLight") {
            DirectionalLights.push_back(Scene->SceneObjects_[i]);
            DirectionalLightIndexes.push_back(i);
        } else if (Scene->SceneObjects_[i].Type_ == "SpotLight") {
            SpotLights.push_back(Scene->SceneObjects_[i]);
            SpotLightIndexes.push_back(i);
        } else if (Scene->SceneObjects_[i].Type_ == "SceneCamera") {
            SceneCameras.push_back(Scene->SceneObjects_[i]);
            SceneCameraIndexes.push_back(i);
        }

    }
    
    




    // Draw Model Entries
    bool ModelTree = ImGui::TreeNodeEx("Models", ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow);

    if (ImGui::BeginPopupContextWindow("Models Context Menu")) {

            if (ImGui::MenuItem("Sort Models Alphabetically")) {

                // Firstly, Rename Models With Same Name To Prevent Issues
                std::vector<std::string> AlreadyUsedNames;
                for (unsigned int i = 0; i < SceneManager_->Scenes_[SceneIndex]->Models.size(); i++) {
                    
                    std::string ModelName = SceneManager_->Scenes_[SceneIndex]->Models[i]->Name;
                    bool AlreadyUsed = false;

                    for (unsigned int NameIndex = 0; NameIndex < AlreadyUsedNames.size(); NameIndex++) {
                        if (AlreadyUsedNames[NameIndex] == ModelName) {
                            AlreadyUsed = true;
                            break;
                        }
                    }

                    if (!AlreadyUsed) {
                        AlreadyUsedNames.push_back(ModelName);
                    } else {
                        SceneManager_->Scenes_[SceneIndex]->Models[i]->Name += "." + std::to_string(i);
                    }


                }

                // Secondly, Create List Of Model Names
                std::vector<std::string> ModelNames;
                for (unsigned int i = 0; i < SceneManager_->Scenes_[SceneIndex]->Models.size(); i++) {
                    ModelNames.push_back(SceneManager_->Scenes_[SceneIndex]->Models[i]->Name);
                }

                // Next, Sort List Of Model Names
                sort(ModelNames.begin(), ModelNames.end());

                // Then, Re-Order Based On Sorted Model Name List
                std::vector<std::shared_ptr<ERS_STRUCT_Model>> SortedList;
                for (unsigned int i = 0; i < ModelNames.size(); i++) {

                    std::string SortedName = ModelNames[i];

                    // Match Names To Models
                    for (unsigned int x = 0; x < SceneManager_->Scenes_[SceneIndex]->Models.size(); x++) {

                        std::string NameToTestAgainst = SceneManager_->Scenes_[SceneIndex]->Models[x]->Name;

                        if (SortedName == NameToTestAgainst) {
                            SortedList.push_back(SceneManager_->Scenes_[SceneIndex]->Models[x]);
                            break;
                        }

                    }

                }
                SceneManager_->Scenes_[SceneIndex]->Models = SortedList;
                SceneManager_->Scenes_[SceneIndex]->HasSelectionChanged = true;


            }

    ImGui::EndPopup();
    }

    if (ModelTree) {

        unsigned int ModelListSize = Scene->Models.size();
        for (unsigned int i = 0; i < ModelListSize; i++) {

            unsigned long IndexInSceneObjects = ModelIndexes[i];


            const char* ObjectName = Scene->SceneObjects_[IndexInSceneObjects].Label_.c_str();
            ImGuiTreeNodeFlags TreeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if ((unsigned long)SelectedSceneObjectIndex == IndexInSceneObjects) {
                TreeFlags |= ImGuiTreeNodeFlags_Selected;
            }
            ImGui::TreeNodeEx((void*)(intptr_t)i, TreeFlags, "%s", ObjectName);


            // If User Clicks Node, Update Object Index
            if (ImGui::IsItemClicked()) {
                Scene->SelectedObject = IndexInSceneObjects;
                Scene->HasSelectionChanged = true;
            }



            // Drag/Drop Target
            long PayloadID;
            if (ImGui::BeginDragDropTarget()) {

                if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("PAYLOAD_ASSET_SCRIPT_ID")) {
                    memcpy(&PayloadID, Payload->Data, sizeof(long));
                    SystemUtils_->Logger_->Log(std::string("Window_SceneTree Recieved Drag Drop Payload 'PAYLOAD_ASSET_SCRIPT_ID' With Value '") + std::to_string(PayloadID) + std::string("'"), 0);
                    
                    // Check If Already In Vector
                    bool Contains = false; 
                    for (unsigned long x = 0; x < Scene->Models[i]->AttachedScriptIndexes_.size(); x++) {
                        if (PayloadID ==  Scene->Models[i]->AttachedScriptIndexes_[x]) {
                            SystemUtils_->Logger_->Log(std::string("Window_SceneTree Error Assigning Payload 'PAYLOAD_ASSET_SCRIPT_ID' To 'Model', Already Attached").c_str(), 0);
                            Contains = true;
                            break;
                        }
                    }

                    if (!Contains) {
                        Scene->Models[i]->AttachedScriptIndexes_.push_back(PayloadID);
                    }
                }

            ImGui::EndDragDropTarget();
            }


            // Context Menu
            if (ImGui::BeginPopupContextItem()) {

                if (ImGui::MenuItem("Rename")) {
                    Subwindow_ModelRenameModal_->Activate(SceneIndex, i);
                } if (ImGui::MenuItem("Duplicate")) {
                    GUI_Windowutil_DuplicateModel(SceneManager_, SceneIndex, i);
                } if (ImGui::MenuItem("Replace All Instances")) {
                    Subwindow_ModelReplaceModal_->Activate(SceneIndex, i);
                } if (ImGui::MenuItem("Auto Number Assets Of This Type")) {

                    // Get Reference Info
                    long AssetIDToMatch = SceneManager_->Scenes_[SceneIndex]->Models[i]->AssetID;
                    std::string BaseName = SceneManager_->Scenes_[SceneIndex]->Models[i]->Name;
                    long CurrentNumber = 1;

                    // Rename And Incriment Current Number
                    for (unsigned int x = 0; x < SceneManager_->Scenes_[SceneIndex]->Models.size(); x++) {
                        long AssetIDToCheck = SceneManager_->Scenes_[SceneIndex]->Models[x]->AssetID;
                        if (AssetIDToCheck == AssetIDToMatch) {
                            SceneManager_->Scenes_[SceneIndex]->Models[x]->Name = BaseName+std::to_string(CurrentNumber);
                            CurrentNumber++;
                        }
                    }

                }
                ImGui::Separator();
                if (ImGui::MenuItem("Delete")) {
                    Subwindow_DeleteModel_->DeleteModel(SceneIndex, i);
                }

            ImGui::EndPopup();
            }


        }

    ImGui::TreePop();
    }


    // Draw Light Entries
    if (ImGui::TreeNodeEx("Lights", ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow)) {

        // Point Lights
        unsigned int PointLightSize = Scene->PointLights.size();
        for (unsigned int i = 0; i < PointLightSize; i++) {

            unsigned long IndexInSceneObjects = PointLightIndexes[i];


            const char* ObjectName = Scene->SceneObjects_[IndexInSceneObjects].Label_.c_str();
            ImGuiTreeNodeFlags TreeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if ((unsigned long)SelectedSceneObjectIndex == IndexInSceneObjects) {
                TreeFlags |= ImGuiTreeNodeFlags_Selected;
            }
            ImGui::TreeNodeEx((void*)(intptr_t)i, TreeFlags, "%s", ObjectName);


            // If User Clicks Node, Update Object Index
            if (ImGui::IsItemClicked()) {
                Scene->SelectedObject = IndexInSceneObjects;
                Scene->HasSelectionChanged = true;
            }


            // Drag/Drop Target
            long PayloadID;
            if (ImGui::BeginDragDropTarget()) {

                if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("PAYLOAD_ASSET_SCRIPT_ID")) {
                    memcpy(&PayloadID, Payload->Data, sizeof(long));
                    SystemUtils_->Logger_->Log(std::string("Window_SceneTree Recieved Drag Drop Payload 'PAYLOAD_ASSET_SCRIPT_ID' With Value '") + std::to_string(PayloadID) + std::string("'"), 0);
                    
                    // Check If Already In Vector
                    bool Contains = false; 
                    for (unsigned long x = 0; x < Scene->PointLights[i]->AttachedScriptIndexes_.size(); x++) {
                        if (PayloadID ==  Scene->PointLights[i]->AttachedScriptIndexes_[x]) {
                            SystemUtils_->Logger_->Log(std::string("Window_SceneTree Error Assigning Payload 'PAYLOAD_ASSET_SCRIPT_ID' To 'PointLight', Already Attached").c_str(), 0);
                            Contains = true;
                            break;
                        }
                    }

                    if (!Contains) {
                        Scene->PointLights[i]->AttachedScriptIndexes_.push_back(PayloadID);
                    }
                }

            ImGui::EndDragDropTarget();
            }


            // Context Menu
            if (ImGui::BeginPopupContextItem(std::string(std::string("PointLight")+std::to_string(i)).c_str())) {

                if (ImGui::MenuItem("Rename")) {
                    Subwindow_PointLightRenameModal_->Activate(SceneIndex, i);
                }
                if (ImGui::MenuItem("Duplicate")) {
                    GUI_Windowutil_DuplicatePointLight(SceneManager_, SceneIndex, i);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Delete")) {
                    Subwindow_DeletePointLight_->DeletePointLight(SceneIndex, i);
                }

            ImGui::EndPopup();
            }



        }

        // Directional Lights
        unsigned long DirectionalLightSize = Scene->DirectionalLights.size();
        for (unsigned int i = 0; i < DirectionalLightSize; i++) {

            unsigned long IndexInSceneObjects = DirectionalLightIndexes[i];


            const char* ObjectName = Scene->SceneObjects_[IndexInSceneObjects].Label_.c_str();
            ImGuiTreeNodeFlags TreeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if ((unsigned long)SelectedSceneObjectIndex == IndexInSceneObjects) {
                TreeFlags |= ImGuiTreeNodeFlags_Selected;
            }
            ImGui::TreeNodeEx((void*)(intptr_t)i, TreeFlags, "%s", ObjectName);


            // If User Clicks Node, Update Object Index
            if (ImGui::IsItemClicked()) {
                Scene->SelectedObject = IndexInSceneObjects;
                Scene->HasSelectionChanged = true;
            }


            // Drag/Drop Target
            long PayloadID;
            if (ImGui::BeginDragDropTarget()) {

                if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("PAYLOAD_ASSET_SCRIPT_ID")) {
                    memcpy(&PayloadID, Payload->Data, sizeof(long));
                    SystemUtils_->Logger_->Log(std::string("Window_SceneTree Recieved Drag Drop Payload 'PAYLOAD_ASSET_SCRIPT_ID' With Value '") + std::to_string(PayloadID) + std::string("'"), 0);
                    
                    // Check If Already In Vector
                    bool Contains = false; 
                    for (unsigned long x = 0; x < Scene->DirectionalLights[i]->AttachedScriptIndexes_.size(); x++) {
                        if (PayloadID ==  Scene->DirectionalLights[i]->AttachedScriptIndexes_[x]) {
                            SystemUtils_->Logger_->Log(std::string("Window_SceneTree Error Assigning Payload 'PAYLOAD_ASSET_SCRIPT_ID' To 'DirectionalLight', Already Attached").c_str(), 0);
                            Contains = true;
                            break;
                        }
                    }

                    if (!Contains) {
                        Scene->DirectionalLights[i]->AttachedScriptIndexes_.push_back(PayloadID);
                    }
                }

            ImGui::EndDragDropTarget();
            }


            // Context Menu
            if (ImGui::BeginPopupContextItem(std::string(std::string("DirectionalLight")+std::to_string(i)).c_str())) {

                if (ImGui::MenuItem("Rename")) {
                    Subwindow_DirectionalLightRenameModal_->Activate(SceneIndex, i);
                }
                if (ImGui::MenuItem("Duplicate")) {
                    GUI_Windowutil_DuplicateDirectionalLight(SceneManager_, SceneIndex, i);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Delete")) {
                    Subwindow_DeleteDirectionalLight_->DeleteDirectionalLight(SceneIndex, i);
                }

            ImGui::EndPopup();
            }



        }

        // Spot Lights
        unsigned long SpotLightSize =  Scene->SpotLights.size();
        for (unsigned int i = 0; i < SpotLightSize; i++) {

            unsigned long IndexInSceneObjects = SpotLightIndexes[i];


            const char* ObjectName = Scene->SceneObjects_[IndexInSceneObjects].Label_.c_str();
            ImGuiTreeNodeFlags TreeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if ((unsigned long)SelectedSceneObjectIndex == IndexInSceneObjects) {
                TreeFlags |= ImGuiTreeNodeFlags_Selected;
            }
            ImGui::TreeNodeEx((void*)(intptr_t)i, TreeFlags, "%s", ObjectName);


            // If User Clicks Node, Update Object Index
            if (ImGui::IsItemClicked()) {
                Scene->SelectedObject = IndexInSceneObjects;
                Scene->HasSelectionChanged = true;
            }


            // Drag/Drop Target
            long PayloadID;
            if (ImGui::BeginDragDropTarget()) {

                if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("PAYLOAD_ASSET_SCRIPT_ID")) {
                    memcpy(&PayloadID, Payload->Data, sizeof(long));
                    SystemUtils_->Logger_->Log(std::string("Window_SceneTree Recieved Drag Drop Payload 'PAYLOAD_ASSET_SCRIPT_ID' With Value '") + std::to_string(PayloadID) + std::string("'"), 0);
                    
                    // Check If Already In Vector
                    bool Contains = false; 
                    for (unsigned long x = 0; x < Scene->SpotLights[i]->AttachedScriptIndexes_.size(); x++) {
                        if (PayloadID ==  Scene->SpotLights[i]->AttachedScriptIndexes_[x]) {
                            SystemUtils_->Logger_->Log(std::string("Window_SceneTree Error Assigning Payload 'PAYLOAD_ASSET_SCRIPT_ID' To 'SpotLight', Already Attached").c_str(), 0);
                            Contains = true;
                            break;
                        }
                    }

                    if (!Contains) {
                        Scene->SpotLights[i]->AttachedScriptIndexes_.push_back(PayloadID);
                    }
                }

            ImGui::EndDragDropTarget();
            }


            // Context Menu
            if (ImGui::BeginPopupContextItem(std::string(std::string("SpotLight")+std::to_string(i)).c_str())) {

                if (ImGui::MenuItem("Rename")) {
                    Subwindow_SpotLightRenameModal_->Activate(SceneIndex, i);
                }
                if (ImGui::MenuItem("Duplicate")) {
                    GUI_Windowutil_DuplicateSpotLight(SceneManager_, SceneIndex, i);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Delete")) {
                    Subwindow_DeleteSpotLight_->DeleteSpotLight(SceneIndex, i);
                }

            ImGui::EndPopup();
            }



        }

    ImGui::TreePop();
    }


    // Draw Scene Camera Entries

    bool SceneCameraTree = ImGui::TreeNodeEx("Cameras", ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow);
    if (SceneCameraTree) {

        unsigned int SceneCameraListSize = Scene->SceneCameras.size();
        for (unsigned int i = 0; i < SceneCameraListSize; i++) {

            unsigned long IndexInSceneObjects = SceneCameraIndexes[i];


            const char* ObjectName = Scene->SceneObjects_[IndexInSceneObjects].Label_.c_str();
            ImGuiTreeNodeFlags TreeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if ((unsigned long)SelectedSceneObjectIndex == IndexInSceneObjects) {
                TreeFlags |= ImGuiTreeNodeFlags_Selected;
            }
            ImGui::TreeNodeEx((void*)(intptr_t)i, TreeFlags, "%s", ObjectName);


            // If User Clicks Node, Update Object Index
            if (ImGui::IsItemClicked()) {
                Scene->SelectedObject = IndexInSceneObjects;
                Scene->HasSelectionChanged = true;
            }



            // Drag/Drop Target
            long PayloadID;
            if (ImGui::BeginDragDropTarget()) {

                if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("PAYLOAD_ASSET_SCRIPT_ID")) {
                    memcpy(&PayloadID, Payload->Data, sizeof(long));
                    SystemUtils_->Logger_->Log(std::string("Window_SceneTree Recieved Drag Drop Payload 'PAYLOAD_ASSET_SCRIPT_ID' With Value '") + std::to_string(PayloadID) + std::string("'"), 0);
                    
                    // Check If Already In Vector
                    bool Contains = false; 
                    for (unsigned long x = 0; x < Scene->SceneCameras[i]->AttachedScriptIndexes_.size(); x++) {
                        if (PayloadID ==  Scene->SceneCameras[i]->AttachedScriptIndexes_[x]) {
                            SystemUtils_->Logger_->Log(std::string("Window_SceneTree Error Assigning Payload 'PAYLOAD_ASSET_SCRIPT_ID' To 'SceneCamera', Already Attached").c_str(), 0);
                            Contains = true;
                            break;
                        }
                    }

                    if (!Contains) {
                        Scene->SceneCameras[i]->AttachedScriptIndexes_.push_back(PayloadID);
                    }
                }

            ImGui::EndDragDropTarget();
            }


            // // Context Menu
            // if (ImGui::BeginPopupContextItem()) {

            //     if (ImGui::MenuItem("Rename")) {
            //         Subwindow_SceneCameraRenameModal_->Activate(SceneIndex, i);
            //     }
            //     ImGui::Separator();
            //     if (ImGui::MenuItem("Delete")) {
            //         Subwindow_DeleteSceneCamera_->DeleteSceneCamera(SceneIndex, i);
            //     }

            // ImGui::EndPopup();
            // }


        }

    ImGui::TreePop();
    }


}

