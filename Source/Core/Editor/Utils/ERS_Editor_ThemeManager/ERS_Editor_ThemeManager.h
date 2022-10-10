//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

#include <imgui.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>


/**
 * @brief This class provides functionality to load and apply themes from yaml files.
 * 
 */
class ERS_CLASS_ThemeManager {


private:

    ERS_LoggingSystem* Logger_; /**<ERS_LoggingSystem Instance*/
    const char*              ThemePath_; /**<Filepath Where Themes Are Located*/

    std::vector<YAML::Node> ThemeFiles_; /**<List Of YAML::Node files*/

    /**
     * @brief Reads a color from the configuration file.
     * 
     * @param NodeName 
     * @param Target 
     * @return ImVec4 
     */
    ImVec4 ReadColor(const char* NodeName, YAML::Node Target);


public:

    ImVec4 ClearColor_; /**Set Clear Color For Background*/
    std::vector<std::string> ThemeNames_; /**<List of theme names (based on filename)*/

    /**
     * @brief Construct a new Theme Manager object
     * 
     * @param Logger_ 
     * @param ThemePath 
     */
    ERS_CLASS_ThemeManager(ERS_LoggingSystem* Logger, const char* ThemePath = "EditorAssets/Configuration/Themes");

    /**
     * @brief Destroy the Theme Manager object
     * 
     */
    ~ERS_CLASS_ThemeManager();

    

    /**
     * @brief Load themes from disk
     * 
     */
    void LoadThemes();

    /**
     * @brief Create the theme menu
     * 
     */
    //FIXME: MOVE MENU UPDATE FUNCTION OUT OF GUP INTO THIS FUNCTION!!!!
    void CreateThemeMenu();

    /**
     * @brief Apply the selected theme
     * 
     * @param ThemeID 
     */
    void ApplyThemes(int ThemeID);

    /**
     * @brief Apply the selected theme
     * 
     * @param ThemeName 
     */
    void ApplyThemes(const char* ThemeName);

};