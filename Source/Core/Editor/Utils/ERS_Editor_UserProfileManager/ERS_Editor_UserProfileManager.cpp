//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_Editor_UserProfileManager.h>


ERS_CLASS_UserProfileManager::ERS_CLASS_UserProfileManager(ERS_LoggingSystem* Logger, const char* UserProfilePath) {

    Logger_ = Logger;
    UserProfilePath_ = std::string(UserProfilePath);
    Logger_->Log("Initializing User Profile Manager", 5);
    LoadUserProfile();

}


ERS_CLASS_UserProfileManager::~ERS_CLASS_UserProfileManager() {

    Logger_->Log("User Profile Destructo Called", 6);

}

void ERS_CLASS_UserProfileManager::LoadUserProfile() {

    Logger_->Log(std::string(std::string("Loading User Profile At Path: ") + UserProfilePath_).c_str(), 4);

    // Load File
    UserProfile_ = YAML::LoadFile(UserProfilePath_.c_str());

    // Decode Profile
    UserFont_ = UserProfile_["DefaultFont"].as<std::string>();
    UserFontSize_ = UserProfile_["DefaultFontSize"].as<float>();
    UserColorProfileName_ = UserProfile_["DefaultColorTheme"].as<std::string>();

}

std::string ERS_CLASS_UserProfileManager::GetUserFont() {
    return UserFont_;
}

int ERS_CLASS_UserProfileManager::GetUserFontSize() {
    return UserFontSize_;
}

std::string ERS_CLASS_UserProfileManager::GetUserColorProfile() {
    return UserColorProfileName_;
}