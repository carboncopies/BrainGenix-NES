//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_Editor_FontManager.h>


ERS_CLASS_FontManager::ERS_CLASS_FontManager(ERS_LoggingSystem* Logger, const char* FontsDirPath) {

    Logger_ = Logger;
    Logger_->Log("Initializing Font Manager", 5);

    FontsDirectoryPath_ = FontsDirPath;
    IndexFonts();

}

ERS_CLASS_FontManager::~ERS_CLASS_FontManager() {
    
    Logger_->Log("Font Manager Destructor Called", 6);

}

// Check String Endings (From: https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c)
bool ERS_CLASS_FontManager::EndsWith(const std::string& Input, const std::string& Ending) {
    if (Ending.size() > Input.size()) return false;
    return std::equal(Input.begin() + Input.size() - Ending.size(), Input.end(), Ending.begin());
}

void ERS_CLASS_FontManager::IndexFonts() {

    FontPathList_.clear();
    FontNameList_.clear();
    Logger_->Log(std::string(std::string("Indexing Fonts In Dir: ") + std::string(FontsDirectoryPath_)).c_str(), 4);


    for (const auto &Entry : std::filesystem::recursive_directory_iterator(std::string(FontsDirectoryPath_))) {

        std::string FilePath{Entry.path().u8string()};
        if ((EndsWith(FilePath, ".ttf")) || (EndsWith(FilePath, ".otf"))) {

            FontPathList_.push_back(FilePath);
            std::string FontName = FilePath.substr(0, strlen(FilePath.c_str())-4);

            // Unix File Systems Use Forward Slash
            int LastSlashIndex = FontName.find_last_of("/");
            FontName = FontName.substr(LastSlashIndex+1, strlen(FontName.c_str())-LastSlashIndex);

            // Because *Windows*, Do It Again With A Backslash
            int LastBackSlashIndex = FontName.find_last_of('\\');
            FontName = FontName.substr(LastBackSlashIndex+1, strlen(FontName.c_str())-LastBackSlashIndex);

            FontNameList_.push_back(FontName); 

            std::string LogText = std::string("Indexed Font: ") + FilePath;
            Logger_->Log(LogText.c_str(), 3);

        }
    
    }

}

void ERS_CLASS_FontManager::UseFont(int FontIndex) {

    const char* FontPath = FontPathList_[FontIndex].c_str();

    Logger_->Log(std::string(std::string("Changing Font To: ") + std::string(FontPath)).c_str(), 4);

    // Load, Apply Font
    ImGuiIO& Io = ImGui::GetIO();

    Io.Fonts->Clear();
    Io.Fonts->AddFontFromFileTTF(FontPath, FontSize_);
    Io.Fonts->Build();
    ImGui_ImplOpenGL3_CreateFontsTexture();

}

void ERS_CLASS_FontManager::UseFont(std::string Font) {

    // Index to Find Font
    int Index;
    bool FoundFont = false;
    for (Index = 0; (long)Index < (long)FontNameList_.size(); Index++) {
        if (FontNameList_[Index] == Font) {
            FoundFont = true;
            break;
        }
    }

    // Apply Font
    if (FoundFont) {

        const char* FontPath = FontPathList_[Index].c_str();

        Logger_->Log(std::string(std::string("Changing Font To: ") + std::string(FontPath)).c_str(), 4);

        ImGuiIO& Io = ImGui::GetIO();
        Io.Fonts->Clear();
        Io.Fonts->AddFontFromFileTTF(FontPath, FontSize_);
        Io.Fonts->Build();
        
    } else {
        Logger_->Log("Failed To Find Target Font, Skipping", 5);
    }

}

void ERS_CLASS_FontManager::SetFontSize(float FontSize) {

    FontSize_ = FontSize;

}

void ERS_CLASS_FontManager::CheckUpdateFont() {

    if (UpdateFont_) {
        UseFont(FontSelector_);
        UpdateFont_ = false;
    }

}

void ERS_CLASS_FontManager::FontSelectorWindow(bool *WindowEnabled) {

    if (*WindowEnabled) {

        ImGuiWindowFlags Flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        if (ImGui::Begin("Font Selector", WindowEnabled, Flags)) {
            ImGui::SetWindowSize(ImVec2(0,0));

                ImGui::BeginChild("Font Radio Buttons", ImVec2(300, 400), true);

                    for (int i = 0; (long)i < (long)FontNameList_.size(); i++) {
                        ImGui::RadioButton(FontNameList_[i].c_str(), &FontSelector_, i);
                    }

                ImGui::EndChild();

                ImGui::Separator();

                ImGui::SliderFloat("Font Size", &FontSize_, 5.0f, 30.0f);

                ImGui::Separator();

                if (ImGui::Button("Reload")) {
                    IndexFonts();
                }
                ImGui::SameLine();

                if (ImGui::Button("Apply")) {
                    UpdateFont_ = true;
                }
                ImGui::SameLine();

                if (ImGui::Button("Close")) {
                    *WindowEnabled = false;
                }


            ImGui::End();
        }

    }

}