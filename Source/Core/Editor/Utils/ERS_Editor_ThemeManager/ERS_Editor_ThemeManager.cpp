//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_Editor_ThemeManager.h>


ERS_CLASS_ThemeManager::ERS_CLASS_ThemeManager(ERS_LoggingSystem* Logger, const char* ThemePath) {

    Logger_ = Logger;
    ThemePath_ = ThemePath;
    Logger_->Log("Initializing Theme Manager", 5);

    LoadThemes();

    // Default To Dark Mode
    int Index;
    for (Index = 0; (long)Index < (long)ThemeNames_.size(); Index++) {
        if (ThemeNames_[Index] == "Dark") {
            break;
        }
    }
    ApplyThemes(Index);

}

ERS_CLASS_ThemeManager::~ERS_CLASS_ThemeManager() {

    Logger_->Log("ERS_CLASS_ThemeManager Destructor Called", 6);

}

void ERS_CLASS_ThemeManager::ApplyThemes(const char* ThemeName) {

    int Index;
    bool HasFoundtheme = false;
    for (Index = 0; (long)Index < (long)ThemeNames_.size(); Index++) {
        if (ThemeNames_[Index] == std::string(ThemeName)) {
            HasFoundtheme = true;
            break;
        }
    }

    if (HasFoundtheme) {
        ApplyThemes(Index);
    } else {
        Logger_->Log("Failed To Find Theme, Skipping", 5);
    }


}

void ERS_CLASS_ThemeManager::LoadThemes() {

    ThemeNames_ = *new std::vector<std::string>;
    ThemeFiles_ = *new std::vector<YAML::Node>;

    // Create List Of Files
    for (const auto &Entry : std::filesystem::directory_iterator(std::string(ThemePath_))) {

        // Get File Path        
        std::string FilePath{Entry.path().u8string()};

        // Load YAML::Node
        YAML::Node Theme = YAML::LoadFile(FilePath.c_str());
        ThemeFiles_.push_back(Theme);

        // Parse Out Name From File Path
        FilePath = FilePath.substr(strlen(ThemePath_)+1);
        FilePath = FilePath.substr(0, strlen(FilePath.c_str())-5);

        // Add To Names Vector
        ThemeNames_.push_back(FilePath);

        // Log Theme Loaded
        Logger_->Log(std::string(std::string("Loaded Editor Theme: ") + FilePath).c_str(), 1);

    }


    Logger_->Log(std::string(std::string("Found ") + std::to_string(ThemeNames_.size()) + std::string(" Themes")).c_str(), 1);

}

ImVec4 ERS_CLASS_ThemeManager::ReadColor(const char* NodeName, YAML::Node Target) {

     Logger_->Log(std::string(std::string("Reading Theme For Value: '") + std::string(NodeName) + std::string("'")).c_str(), 1);

    // Get Subnode
    YAML::Node Subnode = Target[NodeName];

    // Create New Color
    ImVec4 Color;

    // Get Values And Push Into Color
    Color.x = Subnode[0].as<float>()/255.0f;
    Color.y = Subnode[1].as<float>()/255.0f;
    Color.z = Subnode[2].as<float>()/255.0f;
    Color.w = Subnode[3].as<float>()/255.0f;

    return Color;

}

void ERS_CLASS_ThemeManager::ApplyThemes(int ThemeID) {

    // Get Theme Name
    std::string ThemeName = ThemeNames_[ThemeID];
    YAML::Node ThemeNode = ThemeFiles_[ThemeID];

    Logger_->Log(std::string(std::string("Applying Theme: ") + ThemeName).c_str(), 4);


    // Background Color
    Logger_->Log("Reading Theme For Value: 'EditorBackgroundColor'", 1);
    YAML::Node ColorsNode = ThemeNode["EditorBackgroundColor"];

    ClearColor_.x = ColorsNode[0].as<float>()/255.0f;
    ClearColor_.y = ColorsNode[1].as<float>()/255.0f;
    ClearColor_.z = ColorsNode[2].as<float>()/255.0f;
    ClearColor_.w = 1.0f;



    // Setup Style
    ImGuiStyle& Style = ImGui::GetStyle();

    // Hardcoded Transparency
    Style.Alpha = 1.0f;


    // Set Curve Tolerances
    Logger_->Log("Reading Theme For Value: 'CurveTessellationTolerance'", 1);
    Style.CurveTessellationTol = ThemeNode["CurveTessellationTolerance"].as<float>();

    Logger_->Log("Reading Theme For Value: 'CircleTessellationMaxError'", 1);
    Style.CircleTessellationMaxError = ThemeNode["CircleTessellationMaxError"].as<float>();



    // Set Window Title Position
    Logger_->Log("Reading Theme For Value: 'WindowTitlePosition'", 1);
    Style.WindowTitleAlign = ImVec2(ThemeNode["WindowTitlePosition"].as<float>(), 0.50f);
    


    // Set Border Sizes
    Logger_->Log("Reading Theme For Value: 'WindowBorderSize'", 1);
    Style.WindowBorderSize = ThemeNode["WindowBorderSize"].as<float>();

    Logger_->Log("Reading Theme For Value: 'ChildBorderSize'", 1);
    Style.ChildBorderSize = ThemeNode["ChildBorderSize"].as<float>();

    Logger_->Log("Reading Theme For Value: 'PopupBorderSize'", 1);
    Style.PopupBorderSize = ThemeNode["PopupBorderSize"].as<float>();

    Logger_->Log("Reading Theme For Value: 'FrameBorderSize'", 1);
    Style.FrameBorderSize = ThemeNode["FrameBorderSize"].as<float>();

    Logger_->Log("Reading Theme For Value: 'TabBorderSize'", 1);
    Style.TabBorderSize = ThemeNode["TabBorderSize"].as<float>();



    // Set Corner Rounding Parameters
    Logger_->Log("Reading Theme For Value: 'WindowRounding'", 1);
    Style.WindowRounding = ThemeNode["WindowRounding"].as<float>();

    Logger_->Log("Reading Theme For Value: 'ChildRounding'", 1);
    Style.ChildRounding = ThemeNode["ChildRounding"].as<float>();

    Logger_->Log("Reading Theme For Value: 'FrameRounding'", 1);
    Style.FrameRounding = ThemeNode["FrameRounding"].as<float>();

    Logger_->Log("Reading Theme For Value: 'PopupRounding'", 1);
    Style.PopupRounding = ThemeNode["PopupRounding"].as<float>();

    Logger_->Log("Reading Theme For Value: 'ScrollbarRounding'", 1);
    Style.ScrollbarRounding = ThemeNode["ScrollbarRounding"].as<float>();

    Logger_->Log("Reading Theme For Value: 'GrabRounding'", 1);
    Style.GrabRounding = ThemeNode["GrabRounding"].as<float>();

    Logger_->Log("Reading Theme For Value: 'LogSliderDeadzone'", 1);
    Style.LogSliderDeadzone = ThemeNode["LogSliderDeadzone"].as<float>();

    Logger_->Log("Reading Theme For Value: 'TabRounding'", 1);
    Style.TabRounding = ThemeNode["TabRounding"].as<float>();


    // Set Colors
    Style.Colors[ImGuiCol_Text]                  = ReadColor("TextColor", ThemeNode);
    Style.Colors[ImGuiCol_TextDisabled]          = ReadColor("DisabledTextColor", ThemeNode);
    Style.Colors[ImGuiCol_WindowBg]              = ReadColor("WindowBackgroundColor", ThemeNode);
    Style.Colors[ImGuiCol_ChildBg]               = ReadColor("ChildBackgroundColor", ThemeNode);
    Style.Colors[ImGuiCol_PopupBg]               = ReadColor("PopupBackgroundColor", ThemeNode);
    Style.Colors[ImGuiCol_Border]                = ReadColor("BorderColor", ThemeNode);
    Style.Colors[ImGuiCol_BorderShadow]          = ReadColor("BorderShadowColor", ThemeNode);
    Style.Colors[ImGuiCol_FrameBg]               = ReadColor("FrameBackgroundColor", ThemeNode);
    Style.Colors[ImGuiCol_FrameBgHovered]        = ReadColor("FrameBackgroundHoveredColor", ThemeNode);
    Style.Colors[ImGuiCol_FrameBgActive]         = ReadColor("FrameBackgroundActiveColor", ThemeNode);
    Style.Colors[ImGuiCol_TitleBg]               = ReadColor("TitleBackgroundColor", ThemeNode);
    Style.Colors[ImGuiCol_TitleBgCollapsed]      = ReadColor("TitleBackgroundCollapsedColor", ThemeNode);
    Style.Colors[ImGuiCol_TitleBgActive]         = ReadColor("TitleBackgroundActiveColor", ThemeNode);
    Style.Colors[ImGuiCol_MenuBarBg]             = ReadColor("TitleBarColor", ThemeNode);
    Style.Colors[ImGuiCol_ScrollbarBg]           = ReadColor("ScrollbarBackgroundColor", ThemeNode);
    Style.Colors[ImGuiCol_ScrollbarBg]           = ReadColor("ScrollbarBackgroundColor", ThemeNode);
    Style.Colors[ImGuiCol_ScrollbarGrab]         = ReadColor("ScrollbarGrabColor", ThemeNode);
    Style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ReadColor("ScrollbarHoveredColor", ThemeNode);
    Style.Colors[ImGuiCol_ScrollbarGrabActive]   = ReadColor("ScrollbarActiveColor", ThemeNode);
    Style.Colors[ImGuiCol_CheckMark]             = ReadColor("CheckmarkColor", ThemeNode);
    Style.Colors[ImGuiCol_SliderGrab]            = ReadColor("SliderGrabColor", ThemeNode);
    Style.Colors[ImGuiCol_SliderGrabActive]      = ReadColor("SliderGrabActiveColor", ThemeNode);
    Style.Colors[ImGuiCol_Button]                = ReadColor("ButtonColor", ThemeNode);
    Style.Colors[ImGuiCol_ButtonHovered]         = ReadColor("ButtonHoveredColor", ThemeNode);
    Style.Colors[ImGuiCol_ButtonActive]          = ReadColor("ButtonActiveColor", ThemeNode);
    Style.Colors[ImGuiCol_Header]                = ReadColor("HeaderColor", ThemeNode);
    Style.Colors[ImGuiCol_HeaderHovered]         = ReadColor("HeaderHoveredColor", ThemeNode);
    Style.Colors[ImGuiCol_HeaderActive]          = ReadColor("HeaderActiveColor", ThemeNode);
    Style.Colors[ImGuiCol_ResizeGrip]            = ReadColor("ResizeColor", ThemeNode);
    Style.Colors[ImGuiCol_ResizeGripHovered]     = ReadColor("ResizeHoveredColor", ThemeNode);
    Style.Colors[ImGuiCol_ResizeGripActive]      = ReadColor("ResizeActiveColor", ThemeNode);
    Style.Colors[ImGuiCol_PlotLines]             = ReadColor("PlotLinesColor", ThemeNode);
    Style.Colors[ImGuiCol_PlotLinesHovered]      = ReadColor("PlotLinesHoveredColor", ThemeNode);
    Style.Colors[ImGuiCol_PlotHistogram]         = ReadColor("PlotHistogramColor", ThemeNode);
    Style.Colors[ImGuiCol_PlotHistogramHovered]  = ReadColor("PlotHistogramHoveredColor", ThemeNode);
    Style.Colors[ImGuiCol_TextSelectedBg]        = ReadColor("TextSelectedBackgroundColor", ThemeNode);
    Style.Colors[ImGuiCol_DockingPreview]        = ReadColor("DockingPreview", ThemeNode);
    Style.Colors[ImGuiCol_DockingEmptyBg]        = ReadColor("DockingEmptyBackground", ThemeNode);
    Style.Colors[ImGuiCol_Separator]        = ReadColor("SeperatorColor", ThemeNode);
}

