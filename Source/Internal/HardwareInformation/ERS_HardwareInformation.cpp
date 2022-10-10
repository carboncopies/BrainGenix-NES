//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_HardwareInformation.h>



ERS_HardwareInformation::ERS_HardwareInformation(ERS_LoggingSystem* Logger, YAML::Node SystemConfig) {

    Logger_ = Logger;
    SystemConfiguration_ = SystemConfig;
    Logger_->Log("Initializing Hardware Info Subsystem", 5);

    Logger_->Log("Collecting Static Hardware Information", 5);
    Logger_->Log("Getting System Information", 4);
    const auto KernelInfo = iware::system::kernel_info();
    const auto OSInfo = iware::system::OS_info();
    HardwareInfo_.Static_.KernelName = std::string(kernel_variant_name(KernelInfo.variant));
    HardwareInfo_.Static_.OSName = std::string(OSInfo.full_name);

    Logger_->Log(std::string(std::string("Kernel Name: ") + HardwareInfo_.Static_.KernelName).c_str(), 4);
    Logger_->Log(std::string(std::string("Operating System Name: ") + HardwareInfo_.Static_.OSName).c_str(), 4);

    // Get CPU Info
    Logger_->Log("Getting CPU Information", 4);
    const auto CPUQuantities = iware::cpu::quantities();
    HardwareInfo_.Static_.CPUPhysicalCores = CPUQuantities.physical;
    HardwareInfo_.Static_.CPULogicalCores = CPUQuantities.logical;
    HardwareInfo_.Static_.CPUPackages = CPUQuantities.packages;
    HardwareInfo_.Static_.CPUArchitecture = std::string(architecture_name(iware::cpu::architecture()));
    HardwareInfo_.Static_.CPUEndianness = std::string(endianness_name(iware::cpu::endianness()));
    HardwareInfo_.Static_.CPUModelName = std::string(iware::cpu::model_name());
    HardwareInfo_.Static_.CPUModelName = std::string(iware::cpu::vendor_id());

    Logger_->Log(std::string(std::string("Physical CPU Cores: ") + std::to_string(HardwareInfo_.Static_.CPUPhysicalCores)).c_str(), 4);
    Logger_->Log(std::string(std::string("Logical CPU Cores: ") + std::to_string(HardwareInfo_.Static_.CPULogicalCores)).c_str(), 4);
    Logger_->Log(std::string(std::string("Physical CPU Packages: ") + std::to_string(HardwareInfo_.Static_.CPUPackages)).c_str(), 4);
    Logger_->Log(std::string(std::string("CPU Architecture: ") + HardwareInfo_.Static_.CPUArchitecture).c_str(), 4);
    Logger_->Log(std::string(std::string("CPU Endianness: ") + HardwareInfo_.Static_.CPUEndianness).c_str(), 4);
    Logger_->Log(std::string(std::string("CPU Model Name: ") + HardwareInfo_.Static_.CPUModelName).c_str(), 4);
    Logger_->Log(std::string(std::string("CPU Vendor ID: ") + HardwareInfo_.Static_.CPUVendorID).c_str(), 4);

    // Get CPU Cache Info
    for (int i = 0; i < 3; i++) {
        const auto CPUCacheInfo = iware::cpu::cache(i+1);
        HardwareInfo_.Static_.CPUPhysicalCoreCacheSize.push_back(CPUCacheInfo.size);
        HardwareInfo_.Static_.CPUPhysicalCoreCacheLineSize.push_back(CPUCacheInfo.line_size);
        HardwareInfo_.Static_.CPUPhysicalCoreAssociativity.push_back(CPUCacheInfo.associativity);
        HardwareInfo_.Static_.CPUPhysicalCoreType.push_back(cache_type_name(CPUCacheInfo.type));
        
        Logger_->Log(std::string(std::string("Cache Level: ") + std::to_string(i+1)).c_str(), 4);
        Logger_->Log(std::string(std::string("    Cache Size: ") + std::to_string(HardwareInfo_.Static_.CPUPhysicalCoreCacheSize[i])).c_str(), 4);
        Logger_->Log(std::string(std::string("    Cache Line Size: ") + std::to_string(HardwareInfo_.Static_.CPUPhysicalCoreCacheLineSize[i])).c_str(), 4);
        Logger_->Log(std::string(std::string("    Cache Associativity: ") + std::to_string(HardwareInfo_.Static_.CPUPhysicalCoreAssociativity[i])).c_str(), 4);
        Logger_->Log(std::string(std::string("    Cache Type: ") + std::string(HardwareInfo_.Static_.CPUPhysicalCoreType[i])).c_str(), 4);

    }

    // Get GPU Info
    Logger_->Log("Getting GPU Info", 4);
    const auto GPUProperties = iware::gpu::device_properties();

    if (!GPUProperties.empty()) {

        for (auto i = 0u; i < GPUProperties.size(); i++) {

            const auto& GPU = GPUProperties[i];
            HardwareInfo_.Static_.GPUDeviceIDs.push_back(i);
            HardwareInfo_.Static_.GPUVendors.push_back(gpu_vendor_name(GPU.vendor));
            HardwareInfo_.Static_.GPUNames.push_back(GPU.name);
            HardwareInfo_.Static_.GPUVRAMSizes.push_back(GPU.memory_size);
            HardwareInfo_.Static_.GPUCacheSize.push_back(GPU.cache_size);
            HardwareInfo_.Static_.GPUMaxFreq.push_back(GPU.max_frequency);

            Logger_->Log(std::string(std::string("GPU: ") + std::to_string(i)).c_str(), 4);
            Logger_->Log(std::string(std::string("    GPU Vendor: ") + std::string(gpu_vendor_name(GPU.vendor))).c_str(), 4);
            Logger_->Log(std::string(std::string("    GPU Name: ") + std::string(GPU.name)).c_str(), 4);
            Logger_->Log(std::string(std::string("    GPU VRAM Size: ") + std::to_string(GPU.memory_size)).c_str(), 4);
            Logger_->Log(std::string(std::string("    GPU Cache Size: ") + std::to_string(GPU.cache_size)).c_str(), 4);
            Logger_->Log(std::string(std::string("    GPU Max Freq: ") + std::to_string(GPU.max_frequency)).c_str(), 4);


        }

    } else {

        Logger_->Log("No GPU Detection Methods Enabled", 4);

    }




    // Setup Dynamic Info Thread
    Logger_->Log("Setting Up Dynamic Information Refresh Thread", 5);

    // Get Refresh Rate
    float UpdatesPerSecond = SystemConfiguration_["SystemDynamicInformationRefreshRate"].as<float>();
    Logger_->Log("Read Configuration File For 'SystemDynamicInformationRefreshRate' Parameter", 2);

    // Calculate Dynamic Info Refresh Rate Var
    DynamicInfoRefreshRate_ = (1 / UpdatesPerSecond) * 1000.0f;

    // Launch Thread
    Logger_->Log("Starting Dynamic Information Update Thread", 5);
    DynamicUpdateThread_ = SpawnThread();

}


ERS_HardwareInformation::~ERS_HardwareInformation() {

    Logger_->Log("Hardware Information Destructor Called", 6);

    // Shut Down Dynamic Update Thread
    Logger_->Log("Stopping Dynamic Update Thread", 5);
    ShouldDynamicInfoThreadRun_ = false;
    DynamicUpdateThread_.join();

}


std::thread ERS_HardwareInformation::SpawnThread() {

    return std::thread(&ERS_HardwareInformation::DynamicInformationThread, this);

}

ERS_STRUCT_HardwareInfo ERS_HardwareInformation::GetHWInfo() {
    return HardwareInfo_;
}

void ERS_HardwareInformation::DynamicInformationThread() {

    // Name Thread
    SetThreadName("ERS_SysInfo");

    while (ShouldDynamicInfoThreadRun_) {

        // Get Dynamic Info
        GetDynamicInformation();

        // Wait For The WaitTime
        std::this_thread::sleep_for(std::chrono::milliseconds((int)DynamicInfoRefreshRate_));

    }

}

void ERS_HardwareInformation::GetDynamicInformation() {

    // Get Memory Info
    const auto MemoryInfo = iware::system::memory();
    HardwareInfo_.Dynamic_.PhysicalMemoryCapacity = MemoryInfo.physical_total;
    HardwareInfo_.Dynamic_.PhysicalMemoryFree = MemoryInfo.physical_available;
    HardwareInfo_.Dynamic_.SwapCapacity = MemoryInfo.virtual_total;
    HardwareInfo_.Dynamic_.SwapFree = MemoryInfo.virtual_available;

}

// Functions from https://github.com/ThePhD/infoware/blob/main/examples
const char* ERS_HardwareInformation::cache_type_name(iware::cpu::cache_type_t cache_type) noexcept {
	switch(cache_type) {
		case iware::cpu::cache_type_t::unified:
			return "Unified";
		case iware::cpu::cache_type_t::instruction:
			return "Instruction";
		case iware::cpu::cache_type_t::data:
			return "Data";
		case iware::cpu::cache_type_t::trace:
			return "Trace";
		default:
			return "Unknown";
	}
}

const char* ERS_HardwareInformation::architecture_name(iware::cpu::architecture_t architecture) noexcept {
	switch(architecture) {
		case iware::cpu::architecture_t::x64:
			return "x64";
		case iware::cpu::architecture_t::arm:
			return "ARM";
		case iware::cpu::architecture_t::itanium:
			return "Itanium";
		case iware::cpu::architecture_t::x86:
			return "x86";
		default:
			return "Unknown";
	}
}

const char* ERS_HardwareInformation::endianness_name(iware::cpu::endianness_t endianness) noexcept {
	switch(endianness) {
		case iware::cpu::endianness_t::little:
			return "Little-Endian";
		case iware::cpu::endianness_t::big:
			return "Big-Endian";
		default:
			return "Unknown";
	}
}

const char* ERS_HardwareInformation::kernel_variant_name(iware::system::kernel_t variant) noexcept {
	switch(variant) {
		case iware::system::kernel_t::windows_nt:
			return "Windows NT";
		case iware::system::kernel_t::linux:
			return "Linux";
		case iware::system::kernel_t::darwin:
			return "Darwin";
		default:
			return "Unknown";
	}
}

const char* ERS_HardwareInformation::gpu_vendor_name(iware::gpu::vendor_t vendor) noexcept {
	switch(vendor) {
		case iware::gpu::vendor_t::intel:
			return "Intel";
		case iware::gpu::vendor_t::amd:
			return "AMD";
		case iware::gpu::vendor_t::nvidia:
			return "NVidia";
		case iware::gpu::vendor_t::microsoft:
			return "Microsoft";
		case iware::gpu::vendor_t::qualcomm:
			return "Qualcomm";
		case iware::gpu::vendor_t::apple:
			return "Apple";
		default:
			return "Unknown";
	}
}