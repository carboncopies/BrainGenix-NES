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


/**
 * @brief Creates the user profile manager class.
 * 
 */
class ERS_CLASS_UserProfileManager {


private:

    ERS_LoggingSystem*       Logger_;               /**<ERS_LoERS_LoERS_LoggingSystem*/
    YAML::Node               UserProfile_;          /**<YAML::Node Containing User Profile*/
    std::string              UserColorProfileName_; /**<User Color Profile Name*/
    std::string              UserProfilePath_;      /**<Path To User Profile*/
    std::string              UserFont_;             /**<Name Of user fornt*/
    int                      UserFontSize_;         /**<Size of user font*/


    /**
     * @brief Loads a user profile.
     * 
     */
    void LoadUserProfile();


public:

    /**
     * @brief Construct a new User Profile Manager object
     * 
     * @param Logger 
     */
    ERS_CLASS_UserProfileManager(ERS_LoggingSystem* Logger, const char* UserProfilePath = "EditorAssets/Configuration/User/UserConfig.yaml");

    /**
     * @brief Destroy the User Profile Manager object
     * 
     */
    ~ERS_CLASS_UserProfileManager();

    /**
     * @brief Get the User Font object
     * 
     * @return std::string 
     */
    std::string GetUserFont();

    /**
     * @brief Get the User Font Size object
     * 
     * @return int 
     */
    int GetUserFontSize();

    /**
     * @brief Get the User Color Profile object
     * 
     * @return std::string 
     */
    std::string GetUserColorProfile();

};