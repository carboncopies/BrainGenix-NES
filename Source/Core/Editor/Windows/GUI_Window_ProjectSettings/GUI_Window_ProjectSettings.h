//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <memory>
#include <string>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_SystemUtils.h>



class GUI_Window_ProjectSettings {

private:

    char ProjectNameBuffer[512]; /** Project Name Buffer*/
    char ProjectCreationDateBuffer[64]; /** Project Creation Date*/
    char ProjectModificationDateBuffer[64]; /** Project Modification Date*/
    char ProjectVersionBuffer[128]; /**<Project Version Information*/
    char ProjectLicenseNameBuffer[128]; /**<Project License Name String*/
    char ProjectDescriptionBuffer[16384]; /**<Project Description*/
    const char* ProjectScenes[1024]; /**<List of project scene names*/
    int DefaultSceneIndex; /**<Default Scene Index*/
    bool IsProjectFree; /**<Presents A Warning If Propreitary(Non FOSS)*/
    

    ERS_STRUCT_ProjectUtils* ProjectUtils_; /**<Pointer To Project Utils Instance*/
    ERS_STRUCT_SystemUtils* SystemUtils_; /**<System Utils Struct Pointer*/

public:

    bool Enabled_ = false; /**<Is Window Shown Or Hidden*/

public:

    /**
     * @brief Construct a new Window_ProjectSettings object
     * 
     * @param ProjectUtils 
     * @param SystemUtils 
     */
    GUI_Window_ProjectSettings(ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Destroy the Window_ProjectSettings object
     * 
     */
    ~GUI_Window_ProjectSettings();


    /**
     * @brief Draw the window, call this every frame.
     * 
     */
    void Draw();

};