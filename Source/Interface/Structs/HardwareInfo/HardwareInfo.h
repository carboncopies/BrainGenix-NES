//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")



/**
 * @brief This structure is used to store static hardware information.
 * 
 */
struct StaticHardwareInfo {

    // System Info
    std::string KernelName; /**<Name of OS Kernel*/
    std::string OSName; /*<<Name Of OS*/

    // CPU Info
    int CPUPhysicalCores; /**<Number of physical cores in the system (combined across multiple cpus)*/
    int CPULogicalCores; /**<Number of logical cores "threads" in the system (conbined across multiple cpus)*/
    int CPUPackages; /**<Number of physical cpu chips (ex quad-cpu would read 4 here)*/


    std::string CPUArchitecture; /**<CPU architecture as a string*/
    std::string CPUEndianness; /**<Little-Endian or Big-Endian*/
    std::string CPUModelName; /**<Name of model of cpu*/
    std::string CPUVendorID; /**<CPU Vendor ID*/


    std::vector<long> CPUPhysicalCoreCacheSize; /**<Vector containing cache size for all cores*/
    std::vector<long> CPUPhysicalCoreCacheLineSize; /**<Vector containing cache line size for all cores*/
    std::vector<long> CPUPhysicalCoreAssociativity; /**<Vector containing cache associativity for all cores*/
    std::vector<const char*> CPUPhysicalCoreType; /**<Vector containing cache type for all cores*/

    // GPU Info
    std::vector<int> GPUDeviceIDs; /**<List of GPU Device IDs*/
    std::vector<std::string> GPUVendors; /**<List of GPU Vendors*/
    std::vector<std::string> GPUNames; /**<List of GPU Names*/
    std::vector<float> GPUVRAMSizes; /**<List of GPU VRAM Sizes*/
    std::vector<float> GPUCacheSize; /**<List OF GPU Cache Sizes*/
    std::vector<float> GPUMaxFreq; /**<:ist of GPU Max Frequency*/


};


/**
 * @brief This structure is used to store dynamically changing hardware information.
 * 
 */
struct DynamicHardwareInfo {

    // Memory Info
    unsigned long long PhysicalMemoryCapacity; /**<Get Amount Of Memory In System*/
    unsigned long long SwapCapacity; /**<Get Amount Of Swap In System*/
    unsigned long long PhysicalMemoryFree; /**<Get Physical Memory Free*/
    unsigned long long SwapFree; /**<Get Swap Space Free*/



};



/**
 * @brief This structure is used as a container for static and dynamic hardware info structs.
 * 
 */
struct ERS_STRUCT_HardwareInfo {

    // Set Dynamic, Static Info
    StaticHardwareInfo Static_; /**<Structure To Store Static Hardware Information*/
    DynamicHardwareInfo Dynamic_; /**<Structure To Store Dynamic Hardware Information*/

};


