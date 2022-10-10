//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_ScriptEditor.h>


GUI_Window_ScriptEditor::GUI_Window_ScriptEditor(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_CLASS_VisualRenderer* VisualRenderer) {

    SystemUtils_ = SystemUtils;
    ProjectUtils_ = ProjectUtils;
    VisualRenderer_ = VisualRenderer;
    SystemUtils_->Logger_->Log("Initializing GUI ScriptEditor Window", 4);


    Editor_ = std::make_unique<TextEditor>();

    ReloadEditorText(0);
}

GUI_Window_ScriptEditor::~GUI_Window_ScriptEditor() {

    SystemUtils_->Logger_->Log("GUI ScriptEditor Window Destructor Called", 6);

}


void GUI_Window_ScriptEditor::ReloadEditorText(int ScriptIndex) {

    // Perform Sanity Check
    std::string Code;
    if (ScriptIndex >= (int)ProjectUtils_->ProjectManager_->Project_.Scripts.size()) {
        Code = "Script Loading Error";
    } else {
        Code = ProjectUtils_->ProjectManager_->Project_.Scripts[ScriptIndex].Code_;
    }

    // Set Editor Text
    Editor_->SetText(Code);

}


void GUI_Window_ScriptEditor::SaveScript(std::string ScriptText, long AssetID) {

    // Write Data
    std::unique_ptr<ERS_STRUCT_IOData> Data = std::make_unique<ERS_STRUCT_IOData>();
    
    Data->Data.reset(new unsigned char[ScriptText.size()]);
    Data->Size_B = ScriptText.size();
    memcpy(Data->Data.get(), ScriptText.c_str(), ScriptText.size());

    // Write To Storage
    SystemUtils_->ERS_IOSubsystem_->WriteAsset(AssetID, Data.get());


}

void GUI_Window_ScriptEditor::Draw() {

    // // Check Enable Change
    // if (LastEnabledState_ != Enabled_) {

    //     // If Just Enabled
    //     if (Enabled_) {
    //         LivePreviewScriptIndex_ = VisualRenderer_->Scripts_.size();
    //     } else {


    //         // Set Any Viewports Scripts To 0 Who Are Using This Script
    //         for (int i = 0; (long)i < (long)VisualRenderer_->Viewports_.size(); i++) {
    //             if (VisualRenderer_->Viewports_[i]->ScriptIndex == LivePreviewScriptIndex_) {
    //                 VisualRenderer_->Viewports_[i]->ScriptIndex = 0;
    //             }
    //         }

    //         // Remove Script From List
    //         VisualRenderer_->Scripts_.erase(LivePreviewScriptIndex_);


    //     }

    //     // Update Last State
    //     LastEnabledState_ = Enabled_;
    // }

    // Draw Windows
    if (Enabled_) {
    
        DrawEditorWindow();
        DrawToolsWindow();


    }


}



void GUI_Window_ScriptEditor::DrawEditorWindow() {

    bool Visible = ImGui::Begin("Script Editor", &Enabled_, ImGuiWindowFlags_MenuBar);

        // Set Default Window Size
        ImGui::SetWindowSize(ImVec2(600,400), ImGuiCond_FirstUseEver);

        if (Visible) {


            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {


                    // New Script Option
                    if (ImGui::MenuItem("New")) {

                        ERS_STRUCT_Script NewScript;
                        NewScript.AssetID = SystemUtils_->ERS_IOSubsystem_->AllocateAssetID();
                        NewScript.Code_ = "# ERS Script\n";
                        NewScript.Name_ = "Untitled Script";
                        ProjectUtils_->ProjectManager_->Project_.Scripts.push_back(NewScript);

                        SystemUtils_->Logger_->Log(std::string("Creating New Script With Asset ID '") + std::to_string(NewScript.AssetID) + std::string("'"), 0);
                        SaveScript(NewScript.Code_, NewScript.AssetID);

                    }

                    // Program Selector Dropdown
                    if (ImGui::BeginMenu("Open")) {

                        for (unsigned long i = 0; i < ProjectUtils_->ProjectManager_->Project_.Scripts.size(); i++) {

                            std::string ScriptProgramName = ProjectUtils_->ProjectManager_->Project_.Scripts[i].Name_;
                            if (ImGui::MenuItem(ScriptProgramName.c_str())) {

                                // Update Index, REload
                                SelectedScriptProgramIndex_ = i;
                                ReloadEditorText(i);

                            }
                        }


                    ImGui::EndMenu();
                    }

                    // Save Options
                    ImGui::Separator();
                    if (ImGui::MenuItem("Save")) {
                        long ScriptAssetID = ProjectUtils_->ProjectManager_->Project_.Scripts[SelectedScriptProgramIndex_].AssetID;
                        SystemUtils_->Logger_->Log(std::string("Saving Script With Asset ID '") + std::to_string(ScriptAssetID) + std::string("'"), 0);
                        SaveScript(Editor_->GetText(), ScriptAssetID);
                    }

                    // Exit Button
                    ImGui::Separator();
                    if (ImGui::MenuItem("Exit")) {
                        Enabled_ = false;
                    }


                ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit"))
                {
                    bool ro = Editor_->IsReadOnly();
                    if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                        Editor_->SetReadOnly(ro);
                    ImGui::Separator();

                    if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && Editor_->CanUndo()))
                        Editor_->Undo();
                    if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && Editor_->CanRedo()))
                        Editor_->Redo();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, Editor_->HasSelection()))
                        Editor_->Copy();
                    if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && Editor_->HasSelection()))
                        Editor_->Cut();
                    if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && Editor_->HasSelection()))
                        Editor_->Delete();
                    if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                        Editor_->Paste();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Select all", nullptr, nullptr))
                        Editor_->SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(Editor_->GetTotalLines(), 0));

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View"))
                {
                    if (ImGui::MenuItem("Dark palette"))
                        Editor_->SetPalette(TextEditor::GetDarkPalette());
                    if (ImGui::MenuItem("Light palette"))
                        Editor_->SetPalette(TextEditor::GetLightPalette());
                    if (ImGui::MenuItem("Retro blue palette"))
                        Editor_->SetPalette(TextEditor::GetRetroBluePalette());
                    ImGui::EndMenu();
                }
            

            
            ImGui::EndMenuBar();
            }


            // Render Editor
            Editor_->Render("Script Editor");

            // Update Script
            if (ProjectUtils_->ProjectManager_->Project_.Scripts.size() > 0) {
                ProjectUtils_->ProjectManager_->Project_.Scripts[SelectedScriptProgramIndex_].Code_ = Editor_->GetText();
                VisualRenderer_->SetScriptDebug(SelectedScriptProgramIndex_, &DebugLog_);
            }


            // Drag/Drop Target
            long PayloadID;
            if (ImGui::BeginDragDropTarget()) {

                if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("PAYLOAD_ASSET_SCRIPT_ID")) {
                    memcpy(&PayloadID, Payload->Data, sizeof(long));
                    SystemUtils_->Logger_->Log(std::string("Window_ScriptEditor Recieved Drag Drop Payload 'PAYLOAD_ASSET_SCRIPT_ID' With Value '") + std::to_string(PayloadID) + std::string("'"), 0);
                    SelectedScriptProgramIndex_ = PayloadID;
                    ReloadEditorText(PayloadID);

                }

            ImGui::EndDragDropTarget();
            }


        }

    ImGui::End();

}


void GUI_Window_ScriptEditor::DrawToolsWindow() {

    bool CompileVisible = ImGui::Begin("Script Tools", &Enabled_);

        std::string DebugLog = "";

        for (unsigned long i = 0; i < DebugLog_.size(); i++) {
            DebugLog += DebugLog_[i];
        }


        // Set Default Window Size
        ImGui::SetWindowSize(ImVec2(600,400), ImGuiCond_FirstUseEver);


        if (CompileVisible) {

            // Draw Log
            ImGui::BeginChild("Script Log");
            ImGui::TextWrapped("%s", DebugLog.c_str());
            ImGui::EndChild();

        }


 
    ImGui::End();

}


