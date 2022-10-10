//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>

#include <ImGuiFileDialog.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_ModelImportOptions.h>

#include <GUI_Window_ImportProgressBar.h>



/**
 * @brief This class provides the GUI to the import asset option within the file menu.
 *
 */
class GUI_Window_ImportModel {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<used to get access to system utilites like IOmanager, logger, etc.*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_; /**<Project utils struct used to get to the model importer class*/
    GUI_Window_ImportProgressBar* GUI_Window_ImportProgressBar_; /**<Pointer To Window Import Progres Bar*/
    bool AlreadyOpen_ = false; /**<Keeps track of internal window state*/
    bool Working_ = false; /**<Indicates if something is being imported*/ 

    ERS_STRUCT_ModelImportOptions Options_; /**<Import Options For The Model*/

    /**
     * @brief Open File Dialog
     * 
     */
    void OpenFileDialog();

public:

    bool Enabled_ = false; /**<Show/Hide GUI_Window*/

    /**
     * @brief Construct a new gui importasset object.
     * 
     * @param SystemUtils 
     */
    GUI_Window_ImportModel(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, GUI_Window_ImportProgressBar* GUI_Window_ImportProgressBar);

    /**
     * @brief Destroy the gui importasset object.
     * 
     */
    ~GUI_Window_ImportModel();

    /**
     * @brief Update Any Windows
     * 
     */
    void Draw();



};