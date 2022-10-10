//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI_Window_TestEditor.h>



GUI_Window_TestEditor::GUI_Window_TestEditor(ERS_STRUCT_SystemUtils* SystemUtils) {


    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Initializing Demo ImGui Editor Window", 4);

}

GUI_Window_TestEditor::~GUI_Window_TestEditor() {

    SystemUtils_->Logger_->Log("ImGui Editor Window Destructor Called", 6);

}


void GUI_Window_TestEditor::Draw() {

    if (Enabled_) {
    bool Visible = ImGui::Begin("Test Editor Window", &Enabled_, ImGuiWindowFlags_MenuBar);

        // Set Default Window Size
        ImGui::SetWindowSize(ImVec2(300,0), ImGuiCond_FirstUseEver);


        if (Visible) {

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Save"))
                    {
                        auto textToSave = Editor_.GetText();
                        /// save text....
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit"))
                {
                    bool ro = Editor_.IsReadOnly();
                    if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                        Editor_.SetReadOnly(ro);
                    ImGui::Separator();

                    if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && Editor_.CanUndo()))
                        Editor_.Undo();
                    if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && Editor_.CanRedo()))
                        Editor_.Redo();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, Editor_.HasSelection()))
                        Editor_.Copy();
                    if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && Editor_.HasSelection()))
                        Editor_.Cut();
                    if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && Editor_.HasSelection()))
                        Editor_.Delete();
                    if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                        Editor_.Paste();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Select all", nullptr, nullptr))
                        Editor_.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(Editor_.GetTotalLines(), 0));

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View"))
                {
                    if (ImGui::MenuItem("Dark palette"))
                        Editor_.SetPalette(TextEditor::GetDarkPalette());
                    if (ImGui::MenuItem("Light palette"))
                        Editor_.SetPalette(TextEditor::GetLightPalette());
                    if (ImGui::MenuItem("Retro blue palette"))
                        Editor_.SetPalette(TextEditor::GetRetroBluePalette());
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            Editor_.Render("TextEditor");

        }

    ImGui::End();
    }

}