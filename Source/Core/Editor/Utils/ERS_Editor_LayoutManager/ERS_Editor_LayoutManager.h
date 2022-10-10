//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>

struct ERS_STRUCT_EditorLayout {
    int index;
    std::string name;
    std::string IniString;
};

/**
 * @brief Creates the user profile manager class.
 * 
 */
class ERS_CLASS_LayoutManager {


private:

    ERS_LoggingSystem       = nullptr; /**<ERS_LoERS_LoERS_LoggingSystem
    std::string              LayoutDirectory_;           /**<This string stores the path to the editor's layout directory ending in a trailing slash*/

    std::vector<YAML::Node> LayoutFiles_; /**<List Of YAML::Node files*/
    int Index = 0; /**<The index of Layout structs*/
    
public:

    std::vector<std::string> LayoutNames_; /**<List of layout display names (based on display name entry in YAML file)*/
    std::vector<ERS_STRUCT_EditorLayout> Layouts_; /**<List of Layout structs that store the layouts/

    /**
     * @brief Construct a new Layout Manager object
     * 
     * @param Logger 
     */
    ERS_CLASS_LayoutManager(ERS_LoggingSystemnst char* LayoutDirectory = "EditorAssets/Layouts/");

    /**
     * @brief Destroy the Layout Manager object
     * 
     */
    ~ERS_CLASS_LayoutManager();

    /**
    * @brief Save the current layout the user is using.
    *  It will return the set YAML string for the layout
    */
    void SaveLayout(std::string LayoutName);

    /**
    * @brief Load layouts from disk.
    */
    void LoadLayouts();

    /**
    * @brief Apply the selected layout.
    * 
    * @param LayoutID
    */
    void ApplyLayout(int LayoutID);

    /**
    * @brief Apply the selected layout.
    *
    * @param LayoutName
    */
    void ApplyLayout(std::string LayoutName);
};