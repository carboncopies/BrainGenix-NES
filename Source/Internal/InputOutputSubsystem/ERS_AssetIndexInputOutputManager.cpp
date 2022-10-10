//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_AssetIndexInputOutputManager.h>


ERS_AssetIndexInputOutputManager::ERS_AssetIndexInputOutputManager(ERS_LoggingSystem* Logger) {

    Logger_ = Logger;
    Logger_->Log("Initializing ERS Asset Index Input/Output Manager", 5);

}

ERS_AssetIndexInputOutputManager::~ERS_AssetIndexInputOutputManager() {

    Logger_->Log("ERS Asset Index Input/Output Manager Destructor Called", 6);
}


bool ERS_AssetIndexInputOutputManager::LoadAssetIndex(ERS_STRUCT_IOData* Data) {




    Logger_->Log("Loading Asset Index", 4);

    // Decode Asset Index Into YAML::Node
    Logger_->Log("Decoding Asset Index Data From Bytes", 3);
    std::string EncodedString = std::string((const char*)Data->Data.get());
    YAML::Node AssetIndexMetadata;
    try {
        AssetIndexMetadata = YAML::Load(EncodedString);
    } catch (YAML::BadFile&) {
        Logger_->Log("Error Decoding, Invalid YAML Syntax", 9);
        return false;
    }
    Logger_->Log("Finished Decoding Asset Index", 4);


    Lock_.lock();

    // Clear Internal Maps
    Logger_->Log("Clearing Internal Maps For Asset Metadata", 3);
    AssetIDsFound_.erase(AssetIDsFound_.begin(), AssetIDsFound_.end());
    AssetTypeName_.erase(AssetTypeName_.begin(), AssetTypeName_.end());
    AssetCreationDate_.erase(AssetCreationDate_.begin(), AssetCreationDate_.end());
    AssetModificationDate_.erase(AssetModificationDate_.begin(), AssetModificationDate_.end());
    Logger_->Log("Finsihed Clearing Internal Maps", 4);


    // Populate Maps
    Logger_->Log("Populating Asset Index Metadata", 3);
    for (YAML::const_iterator it=AssetIndexMetadata.begin(); it!=AssetIndexMetadata.end(); ++it) {

        Logger_->Log(std::string(std::string("Loading Metadata For Asset With ID ") + std::to_string(it->first.as<long>())).c_str(), 3);

        // Get Asset Metadata Node
        long Index = it->first.as<long>();
        YAML::Node AssetMetadata = it->second;

        // Populate Metadata
        AssetIDsFound_.push_back(Index);
        AssetTypeName_[Index] = {AssetMetadata["AssetType"].as<std::string>()};
        AssetCreationDate_[Index] = {AssetMetadata["AssetCreationDate"].as<std::string>()};
        AssetModificationDate_[Index] = {AssetMetadata["AssetModificationDate"].as<std::string>()};

        if (AssetMetadata["AssetFileName"]) {
            AssetFileName_[Index] = {AssetMetadata["AssetFileName"].as<std::string>()};
        } else {
            Logger_->Log(std::string("Failed To Find Asset Filename Metadata For Asset With ID ") + std::to_string(it->first.as<long>()), 7);
        }


    }
    Logger_->Log("Finished Populating Asset Index Metadata", 4);

    Lock_.unlock();


    return true;

}

bool ERS_AssetIndexInputOutputManager::WriteAssetIndex(ERS_STRUCT_IOData* Data) {

    Logger_->Log("Serializing Asset Database Metadata", 4);

    // Create Yaml Emitter
    YAML::Emitter Metadata;
    Metadata<<YAML::BeginMap;
    for (long i = 0; (long)i < (long)AssetIDsFound_.size(); i++) {
        
        Logger_->Log(std::string(std::string("Serializing Metadata For Asset ") + std::to_string(AssetIDsFound_[i])).c_str(), 3);

        // Add To Metadata
        long CurrentIndex = AssetIDsFound_[i];
        Metadata<<YAML::Key<<CurrentIndex<<YAML::BeginMap;
        
        
        Lock_.lock();
        Metadata<<YAML::Key<<"AssetType"<<YAML::Value<<AssetTypeName_[CurrentIndex];
        Metadata<<YAML::Key<<"AssetCreationDate"<<YAML::Value<<AssetCreationDate_[CurrentIndex];
        Metadata<<YAML::Key<<"AssetModificationDate"<<YAML::Value<<AssetModificationDate_[CurrentIndex];
        Metadata<<YAML::Key<<"AssetFileName"<<YAML::Value<<AssetFileName_[CurrentIndex];
        Lock_.unlock();
        

        Metadata<<YAML::EndMap;

    }
    Metadata<<YAML::EndMap;

    std::string ByteString = std::string(Metadata.c_str());
    Logger_->Log("Finished Serializing Asset DB Metadata", 5);


    // Add To Data Struct
    Logger_->Log("Adding Encoded Metadata String To IOData Struct", 3);
    Data->Data.reset(new unsigned char[ByteString.size()]);
    Data->Size_B = ByteString.size();
    memcpy(Data->Data.get(), ByteString.c_str(), ByteString.size());
    Logger_->Log("Finished Adding Encoded Metadata String To IOData Struct", 4);

    return true;

}

bool ERS_AssetIndexInputOutputManager::UpdateAssetIndex(long AssetID, ERS_STRUCT_IOData* Data) {


    std::string AssetType = Data->AssetTypeName;
    std::string Modified = Data->AssetModificationDate;
    std::string Created = Data->AssetCreationDate;
    std::string FileName = Data->AssetFileName;


    Lock_.lock();
    AssetTypeName_[AssetID] = {AssetType};
    AssetCreationDate_[AssetID] = {Created};
    AssetModificationDate_[AssetID] = {Modified};
    AssetFileName_[AssetID] = {FileName};
    Lock_.unlock();


    // Check If Already In Loaded Assets, If Not, Add
    bool AlreadyInIndex = false;
    for (long i = 0; (long)i < (long)AssetIDsFound_.size(); i++) {
        if (AssetIDsFound_[i] == AssetID) {
            AlreadyInIndex = true;
            break;
        }
    }
    if (!AlreadyInIndex) {
        AssetIDsFound_.push_back(AssetID);
    }


    return true;

}

bool ERS_AssetIndexInputOutputManager::ReadAssetIndex(long AssetID, ERS_STRUCT_IOData* Data) {

    // Lookup Asset Info (If ID Not Zero)
    if (AssetID > 0) {
        Lock_.lock();
        Data->AssetTypeName = AssetTypeName_[AssetID];
        Data->AssetCreationDate = AssetCreationDate_[AssetID];
        Data->AssetModificationDate = AssetModificationDate_[AssetID];
        Lock_.unlock();
    } else {
        Data->AssetTypeName = std::string("Asset Index Metadata");
    }

    return true;

}


