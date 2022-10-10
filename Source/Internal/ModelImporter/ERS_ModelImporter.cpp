//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_ModelImporter.h>


ERS_ModelImporter::ERS_ModelImporter(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Initializing Model Importer Subsystem", 5);

    // Setup Classes
    ModelWriter_ = std::make_unique<ERS_CLASS_ModelWriter>(SystemUtils_->Logger_.get(), SystemUtils_->ERS_IOSubsystem_.get());
    ModelLoader_ = std::make_unique<BrainGenix::ERS::Module::ExternalModelLoader>(SystemUtils_);

    SystemUtils_->Logger_->Log("Starting Asset Import Thread", 4);
    ImportThread_ = std::thread(&ERS_ModelImporter::ImportThread, this);
    SystemUtils_->Logger_->Log("Started Asset Import Thread", 3);

    ERS_ModelWriter_ = std::make_unique<ERS_CLASS_ModelWriter>(
        SystemUtils->Logger_.get(),
        SystemUtils->ERS_IOSubsystem_.get()
    );


}


ERS_ModelImporter::~ERS_ModelImporter() {

    SystemUtils_->Logger_->Log("Asset Importer Backend Destructor Called", 6);

    SystemUtils_->Logger_->Log("Sending Halt Signal To Asset Import Thread", 4);
    BlockThread_.lock();
    StopThread_ = true;
    BlockThread_.unlock();

    SystemUtils_->Logger_->Log("Joining Asset Import Thread", 3);
    ImportThread_.join();

}

// Item Import Thread
void ERS_ModelImporter::ImportThread() {

    // Name Thread
    SetThreadName("ERS_ImportManager");

    while (true) {

        // Check Control Variables
        BlockThread_.lock();
        if (StopThread_) {
            BlockThread_.unlock();
            break;
        }
        BlockThread_.unlock();

        // Check Queue, Import As Needed, Empty Processing Items That Are Done
        LockAssetImportQueue_.lock();
        if (AssetImportQueue_.size() > 0) {

            HasJobFinished_ = false;
            std::string AssetPath = AssetImportQueue_[0];
            bool FlipTextures = AssetQueueFlipTextures_[0];
            AssetImportQueue_.erase(AssetImportQueue_.begin());
            AssetQueueFlipTextures_.erase(AssetQueueFlipTextures_.begin());

            LockAssetImportQueue_.unlock();


                ERS_STRUCT_Model Model;
                ERS_STRUCT_ModelWriterData ModelData;
                ModelData.Model = &Model;
                ModelLoader_->LoadModel(AssetPath, ModelData);
                ModelWriter_->WriteModel(ModelData, FlipTextures);



            LockAssetImportQueue_.lock();
            TotalItemsProcessed_++;
            LockAssetImportQueue_.unlock();
        } else {
            
            HasJobFinished_ = true;
            TotalItemsToImport_ = 0;
            TotalItemsProcessed_ = 0;
            LockAssetImportQueue_.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

    }

}


void ERS_ModelImporter::AddToImportQueue(std::vector<std::string> AssetPaths, std::vector<bool> FlipTextures) {

    SystemUtils_->Logger_->Log("Appending Assets To Asset Import Queue", 5);
    LockAssetImportQueue_.lock();
    HasJobFinished_ = false;
    for (int i = 0; (long)i < (long)AssetPaths.size(); i++) {

        std::string LogStr = std::string("Appending Asset: '") + AssetPaths[i] + std::string("' To Import Queue");
        SystemUtils_->Logger_->Log(LogStr.c_str(), 4);
        AssetImportQueue_.push_back(AssetPaths[i]);
        AssetQueueFlipTextures_.push_back(FlipTextures[i]);
        TotalItemsToImport_ += 1;

    }

    LockAssetImportQueue_.unlock();

}
long ERS_ModelImporter::GetTotalItemsToImport() {
    return TotalItemsToImport_;
}
long ERS_ModelImporter::GetTotalItemsImported() {
    return TotalItemsProcessed_;
}
bool ERS_ModelImporter::HasJobFinished() {
    LockAssetImportQueue_.lock();
    bool Out = HasJobFinished_;
    LockAssetImportQueue_.unlock();
    return Out;
}