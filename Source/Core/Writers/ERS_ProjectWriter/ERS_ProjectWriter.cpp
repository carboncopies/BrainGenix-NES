//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_ProjectWriter.h>



ERS_CLASS_ProjectWriter::ERS_CLASS_ProjectWriter(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils->Logger_->Log("Initializing ERS Project Writer Class", 5);

}

ERS_CLASS_ProjectWriter::~ERS_CLASS_ProjectWriter() {

    SystemUtils_->Logger_->Log("System Destructor Called", 6);

}


bool ERS_CLASS_ProjectWriter::SaveProject(ERS_STRUCT_Project* ProjectPointer, long AssetID) {

    SystemUtils_->Logger_->Log(std::string(std::string("Saving Project To Asset ID ") + std::to_string(AssetID)).c_str(), 5);

    // Serialize Data
    SystemUtils_->Logger_->Log("Serializing Project", 4);

    // Get Current Time In YYYY-MM DD-HH-MM-SS Format
    std::time_t RawCurrentTime;
    std::tm* TimeInformation;
    char TimeBuffer [80];

    std::time(&RawCurrentTime);
    TimeInformation = std::localtime(&RawCurrentTime);

    std::strftime(TimeBuffer, 80, "%Y-%m-%d %H-%M-%S", TimeInformation);
    std::string CurrentTime = std::string(TimeBuffer);

    // Write Data
    YAML::Emitter ProjectEmitter;
    ProjectEmitter<<YAML::BeginMap;
    ProjectEmitter<<YAML::Key<<"ProjectName"<<YAML::Value<<ProjectPointer->ProjectName;
    ProjectEmitter<<YAML::Key<<"ProjectDescription"<<YAML::Value<<ProjectPointer->ProjectDescription;
    ProjectEmitter<<YAML::Key<<"ProjectVersion"<<YAML::Value<<ProjectPointer->ProjectVersion;
    ProjectEmitter<<YAML::Key<<"ProjectCreationDate"<<YAML::Value<<ProjectPointer->ProjectCreationDate;
    ProjectEmitter<<YAML::Key<<"ProjectModificationDate"<<YAML::Value<<CurrentTime.c_str();
    ProjectEmitter<<YAML::Key<<"ProjectCreator"<<YAML::Value<<ProjectPointer->ProjectCreator;
    ProjectEmitter<<YAML::Key<<"ProjectLicense"<<YAML::Value<<ProjectPointer->ProjectLicense;
    ProjectEmitter<<YAML::Key<<"IsLicenseProprietary"<<YAML::Value<<ProjectPointer->IsLicenseProprietary;
    ProjectEmitter<<YAML::Key<<"PlayOnLoad"<<YAML::Value<<ProjectPointer->StartPlayingOnLoad;


    ProjectEmitter<<YAML::Key<<"SceneIDs";
    ProjectEmitter<<YAML::Key<<YAML::BeginMap;
    for (int i = 0; i < (long)ProjectPointer->SceneIDs.size(); i++) {
        ProjectEmitter<<YAML::Key<<i<<ProjectPointer->SceneIDs[i];
    }
    ProjectEmitter<<YAML::EndMap;
    ProjectEmitter<<YAML::Key<<"DefaultScene"<<YAML::Value<<ProjectPointer->DefaultScene;


    ProjectEmitter<<YAML::Key<<"EditorLayouts";
    ProjectEmitter<<YAML::Key<<YAML::BeginMap;
    for (int i = 0; i < (long)ProjectPointer->EditorLayoutIDs.size(); i++) {
        ProjectEmitter<<YAML::Key<<i<<ProjectPointer->EditorLayoutIDs[i];
    }
    ProjectEmitter<<YAML::EndMap;
    ProjectEmitter<<YAML::Key<<"DefaultLayout"<<YAML::Value<<ProjectPointer->DefaultLayout;

    ProjectEmitter<<YAML::Key<<"DefaultShaderProgram"<<YAML::Value<<ProjectPointer->DefaultShaderProgram;
    ProjectEmitter<<YAML::Key<<"ShaderPrograms";
    ProjectEmitter<<YAML::Key<<YAML::BeginMap;
    for (int i = 0; i < (long)ProjectPointer->ShaderPrograms.size(); i++) {
        ProjectEmitter<<YAML::Key<<i;
        ProjectEmitter<<YAML::Key<<YAML::BeginMap;
        ProjectEmitter<<YAML::Key<<"Name"<<YAML::Value<<ProjectPointer->ShaderPrograms[i].Name;
        ProjectEmitter<<YAML::Key<<"VertexID"<<YAML::Value<<ProjectPointer->ShaderPrograms[i].VertexID;
        ProjectEmitter<<YAML::Key<<"FragmentID"<<YAML::Value<<ProjectPointer->ShaderPrograms[i].FragmentID;
        ProjectEmitter<<YAML::Key<<"GeometryID"<<YAML::Value<<ProjectPointer->ShaderPrograms[i].GeometryID;
        ProjectEmitter<<YAML::Key<<"ComputeID"<<YAML::Value<<ProjectPointer->ShaderPrograms[i].ComputeID;
        ProjectEmitter<<YAML::Key<<"TessContolID"<<YAML::Value<<ProjectPointer->ShaderPrograms[i].TCID;
        ProjectEmitter<<YAML::Key<<"TessEvalID"<<YAML::Value<<ProjectPointer->ShaderPrograms[i].TEID;
        ProjectEmitter<<YAML::EndMap;
    }
    ProjectEmitter<<YAML::EndMap;


    SystemUtils_->Logger_->Log("Serializing Script Metadata", 4);
    ProjectEmitter<<YAML::Key<<"Scripts";
    ProjectEmitter<<YAML::Key<<YAML::BeginMap;
    for (unsigned long i = 0; i < ProjectPointer->Scripts.size(); i++) {

        SystemUtils_->Logger_->Log(std::string("Writing Metadata For Script '") + ProjectPointer->Scripts[i].Name_ + std::string("'"), 3);
        ProjectEmitter<<YAML::Key<<i;
        ProjectEmitter<<YAML::Key<<YAML::BeginMap;
        ProjectEmitter<<YAML::Key<<"Name"<<YAML::Value<<ProjectPointer->Scripts[i].Name_;
        ProjectEmitter<<YAML::Key<<"AssetID"<<YAML::Value<<ProjectPointer->Scripts[i].AssetID;
        ProjectEmitter<<YAML::EndMap;

    }
    ProjectEmitter<<YAML::EndMap;


    // Convert Emitter To String
    std::string ProjectByteString = ProjectEmitter.c_str();




    // Write Scripts
    SystemUtils_->Logger_->Log("Writing Scripts", 4);
    for (unsigned long i = 0; i < ProjectPointer->Scripts.size(); i++) {

        SystemUtils_->Logger_->Log(std::string("Writing Data For Script '") + ProjectPointer->Scripts[i].Name_ + std::string("'"), 3);
        std::unique_ptr<ERS_STRUCT_IOData> ScriptData = std::make_unique<ERS_STRUCT_IOData>();
        ScriptData->AssetTypeName = "Script";
        ScriptData->Data.reset(new unsigned char[ProjectPointer->Scripts[i].Code_.size()]);
        ScriptData->Size_B = ProjectPointer->Scripts[i].Code_.size();
        memcpy(ScriptData->Data.get(), ProjectPointer->Scripts[i].Code_.c_str(), ProjectPointer->Scripts[i].Code_.size());
        SystemUtils_->ERS_IOSubsystem_->WriteAsset(ProjectPointer->Scripts[i].AssetID, ScriptData.get());

    }


    // Write To IOData
    SystemUtils_->Logger_->Log("Writing Project To Asset", 4);
    std::unique_ptr<ERS_STRUCT_IOData> ProjectData = std::make_unique<ERS_STRUCT_IOData>();
    
    ProjectData->Data.reset(new unsigned char[ProjectByteString.size()]);
    ProjectData->Size_B = ProjectByteString.size();
    memcpy(ProjectData->Data.get(), ProjectByteString.c_str(), ProjectByteString.size());

    bool Status = SystemUtils_->ERS_IOSubsystem_->WriteAsset(AssetID, ProjectData.get());

    return Status;

}