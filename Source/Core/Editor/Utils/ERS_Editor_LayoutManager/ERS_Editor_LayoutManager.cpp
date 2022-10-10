//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_Editor_LayoutManager.h>
#include <filesystem>
#include <imgui.h>
#include <fstream>

ERS_CLASS_LayoutManager::ERS_CLASS_LayoutManager(ERS_LoggingSystem* Logger, const char* LayoutDirectory) {

    Logger_ = Logger;
    LayoutDirectory_ = LayoutDirectory;
    Logger_->Log("Initializing Layout Manager", 5);

 }


ERS_CLASS_LayoutManager::~ERS_CLASS_LayoutManager() {

    Logger_->Log("Layout Manager Destructor Called", 6);

}

void ERS_CLASS_LayoutManager::LoadLayouts() {

    // Create List Of Files
    for (const auto& Entry : std::filesystem::directory_iterator(std::string(LayoutDirectory_))) {

        // Get File Path
        std::string FilePath{ Entry.path().u8string() };

        // Load YAML::Node
        YAML::Node LayoutNode = YAML::LoadFile(FilePath.c_str());
        
        // Build Temp Layout
        ERS_STRUCT_EditorLayout Layout;
        Layout.index = Index;

        // Parse Out Display Name From File
        std::string LayoutName;
        LayoutName = LayoutNode["DisplayName"].as<std::string>();
        Layout.name = LayoutName;

        // Parse Out Ini String From File
        std::string IniStr;
        IniStr = LayoutNode["ImGuiIni"].as<std::string>();
        Layout.IniString = IniStr;

        // Add To Names and Layouts Vector
        LayoutNames_.push_back(LayoutName);
        Layouts_.push_back(Layout);

        // Log Layout Indexed
        Logger_->Log(std::string(std::string("Indexed Layout: ") + FilePath).c_str(), 1);

        Index++;
    }


    Logger_->Log(std::string(std::string("Found ") + std::to_string(LayoutNames_.size()) + std::string(" Layouts")).c_str(), 1);

}

void ERS_CLASS_LayoutManager::SaveLayout(std::string LayoutName) {

    // Save the Ini String
    std::string IniStr;
    size_t settings_size = 0;
    IniStr = static_cast<std::string> (ImGui::SaveIniSettingsToMemory(&settings_size));

    // Add To Names Vector
    LayoutNames_.push_back(LayoutName);

    // Construct the New Layout Struct
    ERS_STRUCT_EditorLayout newLayout;
    newLayout.index = Index++;
    newLayout.name = LayoutName;
    newLayout.IniString = IniStr;
    Layouts_.push_back(newLayout);

    // Save YAML Node
    YAML::Node Layout;
    Layout["ImGuiIni"] = IniStr;
    Layout["DisplayName"] = LayoutName;

    // Export the YAML string
    YAML::Emitter LayoutYAML;
    LayoutYAML << YAML::BeginMap;

    // Set Constant Info
    LayoutYAML << YAML::Key << "DisplayName" << YAML::Value << LayoutName;
    LayoutYAML << YAML::Key << "ImGuiIni" << YAML::Value << IniStr;

    // Stop Writing, Generate LayoutYAML
    LayoutYAML << YAML::EndMap;
    std::string YAMLstring = std::string(LayoutYAML.c_str());

    // Write the string into a YAML file in the directory
    std::ofstream file(std::string(LayoutDirectory_) + "/" + LayoutName + ".yaml");

    if (!file.fail())
        file << YAMLstring;

    file.close();

}

void ERS_CLASS_LayoutManager::ApplyLayout(std::string LayoutName) {

    int Index;
    bool HasFoundLayout = false;
    for (Index = 0; (long)Index < (long)LayoutNames_.size(); Index++) {
        if (LayoutNames_[Index] == std::string(LayoutName)) {
            HasFoundLayout = true;
            break;
        }
    }

    if (HasFoundLayout) {
        ApplyLayout(Index);
    }
    else {
        Logger_->Log("Failed To Find Layout, Skipping", 5);
    }

}

void ERS_CLASS_LayoutManager::ApplyLayout(int LayoutID) {

    // Get Layout Name
    std::string LayoutName = LayoutNames_[LayoutID];
    ERS_STRUCT_EditorLayout Layout = Layouts_[LayoutID];

    Logger_->Log(std::string(std::string("Applying Layout: ") + LayoutName).c_str(), 4);

    ImGui::LoadIniSettingsFromMemory(Layout.IniString.c_str());

}
