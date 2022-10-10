//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_CLASS_AssetStreamingSystemResourceMonitor.h>


ERS_CLASS_AssetStreamingSystemResourceMonitor::ERS_CLASS_AssetStreamingSystemResourceMonitor(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Starting Asset Streaming Resource Monitoring Subsystem", 5);

    // Initialize Memory Info
    TotalSystemRAM_ = 0;
    TotalSystemVRAM_ = 0;
    UsedSystemRAM_ = 0;
    UsedSystemVRAM_ = 0;

    // Get System Memory Size Info
    SystemUtils_->Logger_->Log("Detecting Hardware Memory Information", 4);
    ERS_STRUCT_HardwareInfo HWInfo = SystemUtils_->ERS_HardwareInformation_->GetHWInfo();

    YAML::Node SystemConfig = *SystemUtils_->LocalSystemConfiguration_;

    // RAM
    if (SystemConfig["OverrideRAM"].as<bool>()) {
        TotalSystemRAM_ = SystemConfig["RAMSizeBytes"].as<long long>();
        SystemUtils_->Logger_->Log(std::string("Using User Set RAM Size Of ") + std::to_string(TotalSystemRAM_) + " Bytes", 3);
    } else {

        // Detect Physical Memory Capacity
        TotalSystemRAM_ = HWInfo.Dynamic_.PhysicalMemoryCapacity;
        SystemUtils_->Logger_->Log(std::string("Detected RAM Size To Be ") + std::to_string(TotalSystemRAM_) + " Bytes", 3);

        // Update The Internal System RAM Limit After Adding The Margin
        SystemUtils_->Logger_->Log("Reading Configuration File For RAM Margin", 4);
        long RAMMargin = SystemConfig["RAMMarginBytes"].as<long long>();
        SystemUtils_->Logger_->Log(std::string("Adding RAM Margin Of ") + std::to_string(RAMMargin) + " Bytes", 4);

        TotalSystemRAM_ -= RAMMargin;

    }
    

    // VRAM
    if (SystemConfig["OverrideVRAM"].as<bool>()) {
        TotalSystemVRAM_ = SystemConfig["VRAMSizeBytes"].as<long long>();
        SystemUtils_->Logger_->Log(std::string("Using User Specified VRAM Size Of ") + std::to_string(TotalSystemVRAM_) + " Bytes", 3);

    } else if (HWInfo.Static_.GPUVRAMSizes.size() > 0) {
        
        // Detect VRAM Capacity
        TotalSystemVRAM_ = HWInfo.Static_.GPUVRAMSizes[0];
        SystemUtils_->Logger_->Log(std::string("Detected VRAM Size To Be ") + std::to_string(TotalSystemVRAM_) + " Bytes", 3);

        // Update The Internal System VRAM Limit After Adding The Margin
        SystemUtils_->Logger_->Log("Reading Configuration File For VRAM Margin", 4);
        long VRAMMargin = SystemConfig["VRAMMarginBytes"].as<long long>();
        SystemUtils_->Logger_->Log(std::string("Adding VRAM Margin Of ") + std::to_string(VRAMMargin) + " Bytes", 4);

        TotalSystemVRAM_ -= VRAMMargin;

    } else {
        TotalSystemVRAM_ = SystemConfig["VRAMSizeBytes"].as<long long>();
        SystemUtils_->Logger_->Log(std::string("Failed To Detect VRAM Size, Using Config Specified VRAM Size Of ") + std::to_string(TotalSystemVRAM_) + " Bytes", 3);
    }

    // Update Renderer Setting Info Struct
    SystemUtils_->RendererSettings_->VRAMBudget_ = TotalSystemVRAM_;
    SystemUtils_->RendererSettings_->RAMBudget_ = TotalSystemRAM_;

}

ERS_CLASS_AssetStreamingSystemResourceMonitor::~ERS_CLASS_AssetStreamingSystemResourceMonitor() {

    SystemUtils_->Logger_->Log("Asset Streaming Subsystem Destructor Invoked", 6);

}

bool ERS_CLASS_AssetStreamingSystemResourceMonitor::TextureFitsInVRAMBudget(unsigned long Size) {

    BlockUpdates_.lock();
 
    // Calculate Free Sizes
    long long HardwareFreeBytes = TotalSystemVRAM_ - UsedSystemVRAM_;
    long long BudgetFreeBytes = SystemUtils_->RendererSettings_->VRAMBudget_ - UsedSystemVRAM_;

    // Perform Comparison To Check If The Texture Would Fit
    bool FitsInHardware = (long long)Size < HardwareFreeBytes;
    bool FitsInBudget = (long long)Size < BudgetFreeBytes;

    BlockUpdates_.unlock();


    // Return Result
    return FitsInBudget && FitsInHardware;
}

void ERS_CLASS_AssetStreamingSystemResourceMonitor::SetTextureVRAMBudget(unsigned long Size) {
    SystemUtils_->RendererSettings_->VRAMBudget_ = Size;
}

void ERS_CLASS_AssetStreamingSystemResourceMonitor::AllocateTextureVRAMFromBudget(unsigned long Size) {
    BlockUpdates_.lock();
    UsedSystemVRAM_ += Size;
    BlockUpdates_.unlock();
}

void ERS_CLASS_AssetStreamingSystemResourceMonitor::DeallocateTextureVRAMFromBudget(unsigned long Size) {
    BlockUpdates_.lock();
    UsedSystemVRAM_ -= Size;
    BlockUpdates_.unlock();
}


bool ERS_CLASS_AssetStreamingSystemResourceMonitor::TextureFitsInRAMBudget(unsigned long Size) {

    BlockUpdates_.lock();

    // Calculate Free Sizes
    long long HardwareFreeBytes = TotalSystemRAM_ - UsedSystemRAM_;
    long long BudgetFreeBytes = SystemUtils_->RendererSettings_->RAMBudget_ - UsedSystemRAM_;

    // Perform Comparison To Check If The Texture Would Fit
    bool FitsInHardware = (long long)Size < HardwareFreeBytes;
    bool FitsInBudget = (long long)Size < BudgetFreeBytes;

    BlockUpdates_.unlock();

    // Return Result
    return FitsInBudget && FitsInHardware;
}

void ERS_CLASS_AssetStreamingSystemResourceMonitor::SetTextureRAMBudget(unsigned long Size) {
    SystemUtils_->RendererSettings_->RAMBudget_ = Size;
}

void ERS_CLASS_AssetStreamingSystemResourceMonitor::AllocateTextureRAMFromBudget(unsigned long Size) {
    BlockUpdates_.lock();
    UsedSystemRAM_ += Size;
    BlockUpdates_.unlock();
}

void ERS_CLASS_AssetStreamingSystemResourceMonitor::DeallocateTextureRAMFromBudget(unsigned long Size) {
    BlockUpdates_.lock();
    UsedSystemRAM_ -= Size;
    BlockUpdates_.unlock();
}

void ERS_CLASS_AssetStreamingSystemResourceMonitor::UpdateTotals() {

    SystemUtils_->RendererSettings_->CurrentRAMUsage_ = UsedSystemRAM_;
    SystemUtils_->RendererSettings_->CurrentVRAMUsage_ = UsedSystemVRAM_;
    
}