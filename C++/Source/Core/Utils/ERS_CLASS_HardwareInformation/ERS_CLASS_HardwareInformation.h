//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <thread>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

#include <infoware/cpu.hpp>
#include <infoware/version.hpp>
#include <infoware/system.hpp>
#include <infoware/pci.hpp>
#include <infoware/gpu.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_CLASS_LoggingSystem.h>
#include <ERS_STRUCT_HardwareInfo.h>


/**
 * @brief Create Hardware Information Class
 * 
 */
class ERS_CLASS_HardwareInformation {

    private:

        // Class Instances
        ERS_CLASS_LoggingSystem* Logger_; /**<Instance Of Logging System*/
        ERS_STRUCT_HardwareInfo HardwareInfo_; /**<Internal Hardware Information Struct*/

        // Config
        YAML::Node SystemConfiguration_; /**MSystem Configuration*/

        // Functions from https://github.com/ThePhD/infoware/blob/main/examples
        const char* gpu_vendor_name(iware::gpu::vendor_t vendor) noexcept;
        const char* kernel_variant_name(iware::system::kernel_t variant) noexcept;
        const char* endianness_name(iware::cpu::endianness_t endianness) noexcept;
        const char* architecture_name(iware::cpu::architecture_t architecture) noexcept;
        const char* cache_type_name(iware::cpu::cache_type_t cache_type) noexcept;

        // Internal Functions
        void GetDynamicInformation(); /**<Updates Dynamic Information*/
        void DynamicInformationThread(); /**Function To Update Info In Background*/
        std::thread SpawnThread(); /**<Create New Thread*/

        // Control Vars
        float DynamicInfoRefreshRate_; /*<Set Number Of ms To wait until next dynamic info refresh*/
        bool ShouldDynamicInfoThreadRun_ = true; /**<Control Variable For Dynamic Info Thread*/

        std::thread DynamicUpdateThread_; /**<Dynamic Update Thread*/

    public:

        /**
         * @brief Construct a new Hardware Information object
         * 
         * @param Logger 
         * @param SystemConfig 
         */
        ERS_CLASS_HardwareInformation(ERS_CLASS_LoggingSystem* Logger, YAML::Node SystemConfig);

        /**
         * @brief Destroy the Hardware Information object
         * 
         */
        ~ERS_CLASS_HardwareInformation();


        /**
         * @brief Get the HardwareInfo Object
         * 
         * @return ERS_STRUCT_HardwareInfo 
         */
        ERS_STRUCT_HardwareInfo GetHWInfo();


};