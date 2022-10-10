//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//


#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <memory>
#include <thread>
#include <string>
#include <mutex>
#include <sstream>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>
#include <TextEditor.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_Script.h>

#include <ERS_CLASS_VisualRenderer.h>






/**
 * @brief This class provides the window for the script editor.
 * 
 */
class GUI_Window_ScriptEditor {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<System Utils Pointer*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_; /**<Project Utils Pointer*/
    ERS_CLASS_VisualRenderer* VisualRenderer_; /**<Instance Of VisualRenderer*/

    std::unique_ptr<TextEditor> Editor_; /**<Editor Instance*/
    std::vector<std::string> DebugLog_; /**<Message Log where tracebacks are put for usage by the script tools window*/
    long SelectedScriptProgramIndex_ = 0; /**<Index of the selected Script program in the project*/
    std::string LastFrameText_; /**Set The Last Text Shown In The Editor*/
    std::shared_ptr<ERS_STRUCT_Script> LivePreviewScript_; /**<Current live preview Script*/
    bool LastEnabledState_ = false; /**<Last State From Last Frame*/
    int LivePreviewScriptIndex_; /**<Index of Live Preview Script*/


private:

    /**
     * @brief Reloads the text from the selected asset id and Script program
     * 
     */
    void ReloadEditorText(int ScriptIndex);

    /**
     * @brief Save the Script to asset data.
     * 
     */
    void SaveScript(std::string ScriptText, long AssetID);

    /**
     * @brief Draws the main editor window
     * 
     */
    void DrawEditorWindow();

    /**
     * @brief Draws the Script tools window
     * 
     */
    void DrawToolsWindow();


public:

    bool Enabled_ = false; /**<Enable/Disable the editor window*/

public:

    /**
     * @brief Construct a new Window_ScriptEditor object
     * 
     * @param SystemUtils 
     */
    GUI_Window_ScriptEditor(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_CLASS_VisualRenderer* VisualRenderer);

    /**
     * @brief Destroy the Window_ScriptEditor object
     * 
     */
    ~GUI_Window_ScriptEditor();

    
    /**
     * @brief Draw the window.
     * 
     */
    void Draw();

};