//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_AssetExplorer.h>


GUI_Window_AssetExplorer::GUI_Window_AssetExplorer(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils) {

    SystemUtils_ = SystemUtils;
    ProjectUtils_ = ProjectUtils;
    SystemUtils_->Logger_->Log("Initializing GUI_Window_AssetExplorer", 5);

    Subwindow_ScriptRenameModal_ = std::make_unique<Subwindow_ScriptRenameModal>(ProjectUtils_);

}

GUI_Window_AssetExplorer::~GUI_Window_AssetExplorer() {

    SystemUtils_->Logger_->Log("GUI_Window_AssetExplorer Destructor Called", 6);


}


void GUI_Window_AssetExplorer::Draw() {

    if (Enabled_) {
    bool Visible = ImGui::Begin("Asset Explorer", &Enabled_);

        // Set Default Window Size
        ImGui::SetWindowSize(ImVec2(300,600), ImGuiCond_FirstUseEver);


        // TODO: Add "selectables" in advanced mode which list all assetids and what they do. perhaps oculd be like this: ID (One-letter-abbreviation for what it does) or an icon if we're feeling fancy
        // add the option to import assets from the explorer into the active scene
        // add the normal mode which only shows ers assets and has names rather than ids
        // add a system to have files/folder abstractions which enables the user to organize their assets under folders, implement drag/drop with this.

        if (Visible) {

            
            if (AdvancedMode_) {
            ImGui::BeginTabBar("Asset Selection Mode");

                // Model Explorer
                if (ImGui::BeginTabItem("Models")) {

                    // Drag + Drop Source WIth List Of Models
                    ImGui::BeginChild("Asset Model Child");

                    // Display Models
                    for (unsigned long i = 0; i < SystemUtils_->ERS_IOSubsystem_->UsedAssetIDs_.size(); i++) {
                        
                        // Check Type
                        unsigned long Key = SystemUtils_->ERS_IOSubsystem_->UsedAssetIDs_[i];
                        std::string Type = SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetTypeName_[Key];
                        std::string FileName = SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetFileName_[Key];
                        if (Type == std::string("Model")) {


                            std::string DisplayName;
                            if (FileName == "") {
                                DisplayName = std::to_string(Key) + std::string(" (Filename Metadata Missing)");
                            } else {
                                DisplayName = FileName;
                            }                  

                            bool Selected = ImGui::Selectable(DisplayName.c_str(), Key == SelectedModelIndex_);
                            if (Selected) {
                                SelectedModelIndex_ = Key;
                            }

                            // Drag+Drop Source
                            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {

                                // Set Drag+Drop Payload
                                ImGui::SetDragDropPayload("PAYLOAD_ASSET_MODEL_ID", &Key, sizeof(long));
                                ImGui::Text("%s", std::string(std::string("ERS Model '") + std::to_string(Key) + std::string("'")).c_str());

                            ImGui::EndDragDropSource();
                            }

                        }
                    }


                    ImGui::EndChild();

                ImGui::EndTabItem();
                }


                // Script Explorer Tab
                if (ImGui::BeginTabItem("Scripts")) {

                    // Drag + Drop Source WIth List Of Scripts
                    ImGui::BeginChild("Asset Script Child");

                    // Display Scripts
                    for (unsigned long i = 0; i < ProjectUtils_->ProjectManager_->Project_.Scripts.size(); i++) {
                        

                        bool Selected = ImGui::Selectable(ProjectUtils_->ProjectManager_->Project_.Scripts[i].Name_.c_str(), i == SelectedScriptIndex_);
                        if (Selected) {
                            SelectedScriptIndex_ = i;
                        }

                        
                        // Context menu, used for renaming and other such functions.
                        if (ImGui::BeginPopupContextItem()) {

                            if (ImGui::MenuItem("Rename")) {
                                Subwindow_ScriptRenameModal_->Activate(i);
                            }

                        ImGui::EndPopup();
                        }

                        // Drag+Drop Source
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {

                            // Set Drag+Drop Payload
                            ImGui::SetDragDropPayload("PAYLOAD_ASSET_SCRIPT_ID", &i, sizeof(long));
                            ImGui::Text("%s", std::string(std::string("ERS Script '") + ProjectUtils_->ProjectManager_->Project_.Scripts[i].Name_ + std::string("'")).c_str());

                        ImGui::EndDragDropSource();
                        }

                        
                    }


                    ImGui::EndChild();

                ImGui::EndTabItem();
                }



                // then, add system to bind scripts to scene objects (drag drop target in scene tree)
                // then add drag drop source
                // then make script editor
                // then make script editor drag/drop target



                // "Advanced" Asset ID Viewer
                if (ImGui::BeginTabItem("Raw Asset IDs")) {

                    // Explorer Child Window
                    ImVec2 IDExplorerSize = ImVec2(ImGui::GetWindowWidth()-300, 0);
                    ImGui::BeginChild("ID Explorer Child Node", IDExplorerSize);

                        // Child Node Title
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Asset ID");
                        ImGui::Separator();

                        // Update Asset ID Selection List
                        int ListLengthDelta = SystemUtils_->ERS_IOSubsystem_->UsedAssetIDs_.size() - AssetIDSelectionList_.size();
                        if (ListLengthDelta > 0) {
                            for (int i = 0; i < ListLengthDelta; i++) {
                                AssetIDSelectionList_.push_back(false);
                            }
                        } else if (ListLengthDelta < 0) {
                            for (int i = 0; i < abs(ListLengthDelta); i++) {
                                AssetIDSelectionList_.pop_back();
                            }
                        }

                        // Iterate Through All Indexed Assets, List In Child Window
                        for (int i = 0; i < (long)SystemUtils_->ERS_IOSubsystem_->UsedAssetIDs_.size(); i++) {
                            bool ItemSelected = ImGui::Selectable(std::to_string(SystemUtils_->ERS_IOSubsystem_->UsedAssetIDs_[i]).c_str(), AssetIDSelectionList_[i]);
                            if (ItemSelected) {
                                AssetIDSelectionList_[i] = !AssetIDSelectionList_[i];
                                LastSelectedIndex_ = i;
                            }
                        }

                    ImGui::EndChild();
                    ImGui::SameLine();
                    ImGui::BeginChild("Description Child Node");

                        // Child Node Title
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Description");
                        ImGui::Separator();

                        // Add Type
                        long SelectedID = SystemUtils_->ERS_IOSubsystem_->UsedAssetIDs_[LastSelectedIndex_];
                        const char* AssetType = SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetTypeName_[SelectedID].c_str();
                        ImGui::Text("Type: %s", AssetType);

                        // Add Dates
                        const char* AssetCreationDate = SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetCreationDate_[SelectedID].c_str();
                        ImGui::Text("Date Created: %s", AssetCreationDate);

                        const char* AssetModificationDate = SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetModificationDate_[SelectedID].c_str();
                        ImGui::Text("Type: %s", AssetModificationDate);


                    ImGui::EndChild();

                ImGui::EndTabItem();
                }

                // Tools bar
                if (ImGui::BeginTabItem("Tools")) {

                    // Child Window
                    ImGui::BeginChild("Asset Explorer Tools");


                    // Heading
                    ImGui::Separator();
                    ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Debugging Tools");
                    ImGui::Separator();


                    // Refresh Button
                    if (ImGui::Button("Refresh Asset Index")) {
                        SystemUtils_->ERS_IOSubsystem_->IndexUsedAssetIDs();
                    }

                    if (ImGui::Button("Repair Asset Metadata")) {


                        SystemUtils_->Logger_->Log("Attempting To Repairing Asset Metadata", 5);

                        // Iterate Over All Assets Known
                        for (unsigned int i = 0; i < SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetIDsFound_.size(); i++) {
                            
                            // Setup Metadata
                            std::string AssetType = "";
                            std::string AssetFileName = "";
                            long AssetID = SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetIDsFound_[i];
                            SystemUtils_->Logger_->Log(std::string("Repairing For Asset ") + std::to_string(AssetID), 4);


                            // Try And Find Match WIth Either Texture, Model, Script, Shader, Etc...
                            for (unsigned int x = 0; x < ProjectUtils_->SceneManager_->Scenes_.size(); x++) {


                                // Try And Find Match With Model Asset ID
                                for (unsigned int y = 0; y < ProjectUtils_->SceneManager_->Scenes_[x]->Models.size(); y++) {
                                    if (ProjectUtils_->SceneManager_->Scenes_[x]->Models[y]->AssetID == AssetID) {
                                        SystemUtils_->Logger_->Log("Found AssetID Match In Scene Models", 2);
                                        AssetType = "Model";
                                        AssetFileName =  ProjectUtils_->SceneManager_->Scenes_[x]->Models[y]->Name;
                                        break;
                                    }

                                }







                                // Check If Match Found
                                if (AssetType != "") {
                                    break;
                                }


                            }
                            

                            // Update Metadata Lists (Skipping If Data Is Already There)
                            if (SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetFileName_[AssetID] == "") {
                                SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetFileName_[AssetID] = AssetFileName;
                                SystemUtils_->Logger_->Log(std::string("Updating Metadata Property 'AssetFileName' For Asset '") + std::to_string(AssetID) + std::string("' To '") + AssetFileName + std::string("'"), 3);
                            } else {
                                SystemUtils_->Logger_->Log(std::string("Asset Already Has Metadata Property 'AssetFileName' For Asset '") + std::to_string(AssetID)
                                + std::string("' Of Value '") + SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetFileName_[AssetID] + std::string("'"), 3);
                            }
                            if (SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetTypeName_[AssetID] == "" || SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetTypeName_[AssetID] == "Undefined") {
                                SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetTypeName_[AssetID] = AssetType;
                                SystemUtils_->Logger_->Log(std::string("Updating Metadata Property 'AssetType' For Asset '") + std::to_string(AssetID) + std::string("' To '") + AssetType + std::string("'"), 3);
                            } else {
                                SystemUtils_->Logger_->Log(std::string("Asset Already Has Metadata Property 'AssetType' For Asset '") + std::to_string(AssetID)
                                + std::string("' Of Value '") + SystemUtils_->ERS_IOSubsystem_->AssetIndexIOManager_->AssetTypeName_[AssetID] + std::string("'"), 3);
                            }
                            


                        }


                        SystemUtils_->Logger_->Log("Asset Metadata Repair Attempt Complete", 5);


                    }

                    // End Child
                    ImGui::EndChild();

                ImGui::EndTabItem();
                }

            ImGui::EndTabBar();
            }


        }

    ImGui::End();
    }


    Subwindow_ScriptRenameModal_->Draw();


}