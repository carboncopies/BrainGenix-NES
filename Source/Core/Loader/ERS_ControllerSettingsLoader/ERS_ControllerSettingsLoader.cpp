//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_ControllerSettingsLoader.h>


ERS_CLASS_ControllerSettingsLoader::ERS_CLASS_ControllerSettingsLoader(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Initializing ERS Controller Settings Loader", 5);

}


ERS_CLASS_ControllerSettingsLoader::~ERS_CLASS_ControllerSettingsLoader() {

    SystemUtils_->Logger_->Log("ERS Controller Settings Loader Destructor Called", 4);

}


bool ERS_CLASS_ControllerSettingsLoader::LoadControllerSettings(ERS_STRUCT_ControllerSettings* ControllerSettings, long AssetID) {

    SystemUtils_->Logger_->Log(std::string(std::string("Loading ERS Controller Settings From AssetID: ") + std::to_string(AssetID)).c_str(), 4);

    // Read Data
    std::unique_ptr<ERS_STRUCT_IOData> SettingsRawData = std::make_unique<ERS_STRUCT_IOData>();
    SystemUtils_->ERS_IOSubsystem_->ReadAsset(AssetID, SettingsRawData.get());
    std::string SettingsEncodedString = std::string((const char*)SettingsRawData->Data.get());
    

    // Attempt Loading 
    YAML::Node SettingsData;
    try {
        SettingsData = YAML::Load(SettingsEncodedString);
    } catch(YAML::BadFile&) {
        SystemUtils_->Logger_->Log("Error Loading, Invalid YAML Syntax", 9);
        return false;
    }


    // Try Loading From YAML
    try {

        // Populate Settings Params
        ControllerSettings->ControllerName = SettingsData["ControllerName"].as<std::string>();
        ControllerSettings->SettingsProfileName = SettingsData["SettingsProfileName"].as<std::string>();


        ControllerSettings->JoystickLeftXGain = SettingsData["JoystickLeftXGain"].as<float>();
        ControllerSettings->JoystickLeftYGain = SettingsData["JoystickLeftYGain"].as<float>();

        ControllerSettings->JoystickRightXGain = SettingsData["JoystickRightXGain"].as<float>();
        ControllerSettings->JoystickRightYGain = SettingsData["JoystickRightYGain"].as<float>();


        ControllerSettings->MinThresholdJoystickLeftX = SettingsData["MinThresholdJoystickLeftX"].as<float>();
        ControllerSettings->MinThresholdJoystickLeftY = SettingsData["MinThresholdJoystickLeftY"].as<float>();
        ControllerSettings->MinThresholdJoystickRightX = SettingsData["MinThresholdJoystickLeftX"].as<float>();
        ControllerSettings->MinThresholdJoystickRightY = SettingsData["MinThresholdJoystickRightY"].as<float>();

        ControllerSettings->MaxThresholdJoystickLeftX = SettingsData["MaxThresholdJoystickLeftX"].as<float>();
        ControllerSettings->MaxThresholdJoystickLeftY = SettingsData["MaxThresholdJoystickLeftY"].as<float>();
        ControllerSettings->MaxThresholdJoystickRightX = SettingsData["MaxThresholdJoystickRightX"].as<float>();
        ControllerSettings->MaxThresholdJoystickRightY = SettingsData["MaxThresholdJoystickRightY"].as<float>();


        ControllerSettings->JoystickLeftButtonIndex = SettingsData["JoystickLeftButtonIndex"].as<int>();
        ControllerSettings->JoystickRightButtonIndex = SettingsData["JoystickRightButtonIndex"].as<int>();
        ControllerSettings->BackButtonIndex = SettingsData["BackButtonIndex"].as<int>();
        ControllerSettings->MenuButtonIndex = SettingsData["MenuButtonIndex"].as<int>();
        ControllerSettings->OptionsButtonIndex = SettingsData["OptionsButtonIndex"].as<int>();
        ControllerSettings->TriangleButtonIndex = SettingsData["TriangleButtonIndex"].as<int>();
        ControllerSettings->SquareButtonIndex = SettingsData["SquareButtonIndex"].as<int>();
        ControllerSettings->CircleButtonIndex = SettingsData["CircleButtonIndex"].as<int>();
        ControllerSettings->CrossButtonIndex = SettingsData["CrossButtonIndex"].as<int>();
        ControllerSettings->RightBumperButtonIndex = SettingsData["RightBumperButtonIndex"].as<int>();
        ControllerSettings->LeftBumperButtonIndex = SettingsData["LeftBumperButtonIndex"].as<int>();
        ControllerSettings->DPADUpButtonIndex = SettingsData["DPADUpButtonIndex"].as<int>();
        ControllerSettings->DPADDownButtonIndex = SettingsData["DPADDownButtonIndex"].as<int>();
        ControllerSettings->DPADLeftButtonIndex = SettingsData["DPADLeftButtonIndex"].as<int>();
        ControllerSettings->DPADRightButtonIndex = SettingsData["DPADRightButtonIndex"].as<int>();

    } catch(YAML::BadSubscript&) {
        
        SystemUtils_->Logger_->Log("Error Loading Controller Configuration, Invalid Parameter(s), Will Use Default Instead", 9);
        
        // Use Defaults
        ControllerSettings->ControllerName = std::string("*");
        ControllerSettings->SettingsProfileName = std::string("Default Layout");


        ControllerSettings->JoystickLeftXGain = 1.0f;
        ControllerSettings->JoystickLeftYGain = 1.0f;

        ControllerSettings->JoystickRightXGain = 1.0f;
        ControllerSettings->JoystickRightYGain = 1.0f;


        ControllerSettings->MinThresholdJoystickLeftX = 0.0f;
        ControllerSettings->MinThresholdJoystickLeftY = 0.0f;
        ControllerSettings->MinThresholdJoystickRightX = 0.0f;
        ControllerSettings->MinThresholdJoystickRightY = 0.0f;

        ControllerSettings->MaxThresholdJoystickLeftX = 1.0f;
        ControllerSettings->MaxThresholdJoystickLeftY = 1.0f;
        ControllerSettings->MaxThresholdJoystickRightX = 1.0f;
        ControllerSettings->MaxThresholdJoystickRightY = 1.0f;


        ControllerSettings->JoystickLeftButtonIndex = 9;
        ControllerSettings->JoystickRightButtonIndex = 10;
        ControllerSettings->BackButtonIndex = 6;
        ControllerSettings->MenuButtonIndex = 7;
        ControllerSettings->OptionsButtonIndex = 8;
        ControllerSettings->TriangleButtonIndex = 3;
        ControllerSettings->SquareButtonIndex = 2;
        ControllerSettings->CircleButtonIndex = 1;
        ControllerSettings->CrossButtonIndex = 0;
        ControllerSettings->RightBumperButtonIndex = 5;
        ControllerSettings->LeftBumperButtonIndex = 4;
        ControllerSettings->DPADUpButtonIndex = 11;
        ControllerSettings->DPADDownButtonIndex = 12;
        ControllerSettings->DPADLeftButtonIndex = 13;
        ControllerSettings->DPADRightButtonIndex = 14;
        return false;
    }    
    return true;

}