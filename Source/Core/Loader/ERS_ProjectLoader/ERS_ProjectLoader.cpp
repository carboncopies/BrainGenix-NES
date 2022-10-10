//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_ProjectLoader.h>


ERS_CLASS_ProjectLoader::ERS_CLASS_ProjectLoader(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;

    SystemUtils_->Logger_->Log("Initializing Project Loader", 5);

}

ERS_CLASS_ProjectLoader::~ERS_CLASS_ProjectLoader() {

    SystemUtils_->Logger_->Log("ERS_CLASS_ProjectLoader Destructor Called", 6);

}

ERS_STRUCT_Project ERS_CLASS_ProjectLoader::LoadProject(long AssetID) {

    // Log Project Loading
    SystemUtils_->Logger_->Log(std::string(std::string("Loading Project With Asset ID ") + std::to_string(AssetID)).c_str(), 4);
    std::unique_ptr<ERS_STRUCT_IOData> ProjectData = std::make_unique<ERS_STRUCT_IOData>();
    SystemUtils_->ERS_IOSubsystem_->ReadAsset(AssetID, ProjectData.get());

    // Decode YAML
    std::string DataString = std::string((const char*)ProjectData->Data.get());
    YAML::Node ProjectNode = YAML::Load(DataString);



    // Create New Struct
    ERS_STRUCT_Project Project;
    Project.ControllerSettings = new std::vector<ERS_STRUCT_ControllerSettings>();

    // Populate Struct Metadata
    Project.ProjectName = ProjectNode["ProjectName"].as<std::string>();
    Project.ProjectDescription = ProjectNode["ProjectDescription"].as<std::string>();
    Project.ProjectVersion = ProjectNode["ProjectVersion"].as<std::string>();
    
    Project.ProjectCreationDate = ProjectNode["ProjectCreationDate"].as<std::string>();
    Project.ProjectModificationDate = ProjectNode["ProjectModificationDate"].as<std::string>();

    Project.ProjectCreator = ProjectNode["ProjectCreator"].as<std::string>();
    Project.ProjectLicense = ProjectNode["ProjectLicense"].as<std::string>();
    Project.IsLicenseProprietary = ProjectNode["IsLicenseProprietary"].as<bool>();
    
    Project.DefaultLayout = ProjectNode["DefaultLayout"].as<int>();
    Project.DefaultScene = ProjectNode["DefaultScene"].as<int>();
    Project.DefaultShaderProgram = ProjectNode["DefaultShaderProgram"].as<int>();
    
    if (ProjectNode["PlayOnLoad"]) {
        Project.StartPlayingOnLoad = ProjectNode["PlayOnLoad"].as<bool>();
    } else {
        Project.StartPlayingOnLoad = false;
        SystemUtils_->Logger_->Log("Project Metadata Missing 'PlayOnLoad' Param, Defaulting to 'FALSE'", 7);
    }


    // Populate Scene IDs
    YAML::Node ProjectSceneIDs = ProjectNode["SceneIDs"];
    for (YAML::const_iterator it=ProjectSceneIDs.begin(); it!=ProjectSceneIDs.end(); ++it) {
        SystemUtils_->Logger_->Log(std::string(std::string("Identified Scene In Project With ID: ") + std::to_string(it->second.as<long>())).c_str(), 3);
        Project.SceneIDs.push_back(it->second.as<long>());
    }

    // Populate Layouts
    YAML::Node EditorLayouts = ProjectNode["EditorLayouts"];
    for (YAML::const_iterator it=EditorLayouts.begin(); it!=EditorLayouts.end(); ++it) {
        SystemUtils_->Logger_->Log(std::string(std::string("Identified Layout Project With ID: ") + std::to_string(it->second.as<long>())).c_str(), 3);
        Project.EditorLayoutIDs.push_back(it->second.as<long>());
    }

    // Populate Controller Settings
    YAML::Node ControllerSettings = ProjectNode["ControllerSettings"];
    for (YAML::const_iterator it=ControllerSettings.begin(); it!=ControllerSettings.end(); ++it) {
        SystemUtils_->Logger_->Log(std::string(std::string("Identified Controller Settings Map With ID: ") + std::to_string(it->second.as<long>())).c_str(), 3);
        Project.GameControllerSettingsIDs.push_back(it->second.as<long>());
    }

    // Populate Shader Structs
    SystemUtils_->Logger_->Log(std::string(std::string("Loading Shader Programs From Project Asset")).c_str(), 3);
    YAML::Node ShadersPrograms = ProjectNode["ShaderPrograms"];
    for (YAML::const_iterator it=ShadersPrograms.begin(); it!=ShadersPrograms.end(); ++it) {
        ERS_STRUCT_ShaderProgramAssetIDs Program;
        Program.Name = it->second["Name"].as<std::string>();
        Program.VertexID = it->second["VertexID"].as<long>();
        Program.FragmentID = it->second["FragmentID"].as<long>();

        if (it->second["GeometryID"]) {
            Program.GeometryID = it->second["GeometryID"].as<long>();
        } else {
            Program.GeometryID = -1;
        }

        if (it->second["ComputeID"]) {
            Program.ComputeID = it->second["ComputeID"].as<long>();
        } else {
            Program.ComputeID = -1;
        }

        if (it->second["TessControlID"]) {
            Program.TCID = it->second["TessControlID"].as<long>();
        } else {
            Program.TCID = -1;
        }
        
        if (it->second["TessEvalID"]) {
            Program.TEID = it->second["TessEvalID"].as<long>();
        } else {
            Program.TEID = -1;
        }
        
        

        Project.ShaderPrograms.push_back(Program);
    }
    SystemUtils_->Logger_->Log(std::string(std::string("Finished Loading Shader Programs")).c_str(), 4);

    // Populate Script Structs With Metadata
    if (ProjectNode["Scripts"]) {
        SystemUtils_->Logger_->Log("Loading Project Script Metadata", 5);
        YAML::Node Scripts = ProjectNode["Scripts"];
        for (YAML::const_iterator it=Scripts.begin(); it!=Scripts.end(); ++it) {
            ERS_STRUCT_Script Script;
            Script.Name_ = it->second["Name"].as<std::string>();
            Script.AssetID = it->second["AssetID"].as<long>();
            Project.Scripts.push_back(Script);
        }
    } else {
        SystemUtils_->Logger_->Log("Project Script Metadata Missing", 7);
    }


    // Load Script(s)
    SystemUtils_->Logger_->Log("Loading Project Scripts", 5);
    for (unsigned long i = 0; i < Project.Scripts.size(); i++) {
        
        ERS_STRUCT_Script* Script = &Project.Scripts[i];

        std::string LogStr = std::string("Loading Script '") + std::to_string(i) + std::string("' of '") + std::to_string(Project.Scripts.size()) + std::string("'");
        SystemUtils_->Logger_->Log(LogStr, 4);


        std::unique_ptr<ERS_STRUCT_IOData> ScriptData = std::make_unique<ERS_STRUCT_IOData>();
        SystemUtils_->ERS_IOSubsystem_->ReadAsset(Script->AssetID, ScriptData.get());
        Script->Code_ = std::string((const char*)ScriptData->Data.get());

        LogStr = std::string("Loaded Script '") + Script->Name_ + std::string("'");
        SystemUtils_->Logger_->Log(LogStr, 3);
    }


    // Return Struct When Populated
    return Project;

}
