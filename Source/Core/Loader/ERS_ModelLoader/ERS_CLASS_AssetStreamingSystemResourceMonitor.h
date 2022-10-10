//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>
#include <vector>
#include <thread>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_HardwareInfo.h>


/**
 * @brief This class gets information about how the system is performing to influence the streaming manager.
 * Essentially, it just informs the manager when we're about to run out of memory to ensure that we can unload things as needed.
 * Additionally, it does the opposite - informing us when we have more memory so that we know when we can load more textures into CPU RAM.
 * This class gets the CPU/System RAM info from the Hardware Information Class, and processes that info to make 'reccomendataions' to the manager.
 * 
 */
class ERS_CLASS_AssetStreamingSystemResourceMonitor {

private:

    ERS_STRUCT_SystemUtils* SystemUtils_; /**<Struct containing essential services such as logging and Asset IO*/

    std::mutex BlockUpdates_; /**<Used to ensure thread-safety when updating the system*/

    // Total Amounts Assumed For System RAM and VRAM In Bytes
    unsigned long long TotalSystemRAM_  = 0; /**<Number of bytes of RAM that the system has*/
    unsigned long long TotalSystemVRAM_ = 0; /**<Number of bytes of VRAM that the system has*/

    // Used Amounts
    unsigned long long UsedSystemRAM_   = 0; /**<Number of bytes of RAM used by textures*/
    unsigned long long UsedSystemVRAM_  = 0; /**<Number of bytes of VRAM used by textures*/
    


public:

    /**
     * @brief Sets up the resource monitor for the asset streaming service.
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_AssetStreamingSystemResourceMonitor(ERS_STRUCT_SystemUtils* SystemUtils);

    /**
     * @brief Destroy the ers class assetstreamingsystemresourcemonitor object
     * 
     */
    ~ERS_CLASS_AssetStreamingSystemResourceMonitor();

    /**
     * @brief Determines if we can both fit the given texture in VRAM and within our self-imposed texture budget.
     * The actual vram limit is pretty self-explanitory. We just want to make sure that there's still space for the given texture.
     * The self imposed limit regarding texture budget allows us to ensure we don't eat all the system's resources - leading to a crash.
     * 
     * The texture VRAM budget can be configured via the SetTextureVRAMBudget method, or by adjusting the rendering settings in the systemutils struct.
     * 
     * @param NumberBytes 
     * @return true 
     * @return false 
     */
    bool TextureFitsInVRAMBudget(unsigned long NumberBytes);

    /**
     * @brief This allows us to update the current texture VRAM budget.
     * If a number is larger than the systems memory, that limit will apply first so setting it too high is okay.
     * 
     * @param NewTextureVRAMBudget 
     */
    void SetTextureVRAMBudget(unsigned long NewTextureVRAMBudget);

    /**
     * @brief Allows us to keep track of what VRAM we've used.
     * Remember to deallocate here when you destroy the texture!
     * 
     * @param Size 
     */
    void AllocateTextureVRAMFromBudget(unsigned long Size);

    /**
     * @brief Dallocates previously allocated vram from the budget.
     * Again, make sure you have the right size here so we don't loose track of how much VRAM we've used.
     * 
     * @param Size 
     */
    void DeallocateTextureVRAMFromBudget(unsigned long Size);



    /**
     * @brief Determines if we can fit the given texture into regular system memory and within our self-imposed budget.
     * This is basically the same as 'TextureFitsInVRAMBudget' but for system memory.
     * 
     * Again, the budget can be adjusted with the associated SetTextureRAMBudget call or via the rendering settings strut in SystemUtils.
     * 
     * @param NumberBytes 
     * @return true 
     * @return false 
     */
    bool TextureFitsInRAMBudget(unsigned long NumberBytes);

    /**
     * @brief This does the same thing that set vram budget does but for the system's RAM budget.
     * We require that this be larger than the VRAM budget due to some optimizations we've made internally.
     * Ideally this RAM budget should be around 4X larger than the VRAM budget.
     * 
     * @param NewTextureRAMBudget 
     */
    void SetTextureRAMBudget(unsigned long NewTextureRAMBudget);

    /**
     * @brief Allows us to keep track of what RAM we've used.
     * Remember to deallocate here when you destroy the texture!
     * 
     * @param Size 
     */
    void AllocateTextureRAMFromBudget(unsigned long Size);

    /**
     * @brief Dallocates previously allocated ram from the budget.
     * Again, make sure you have the right size here so we don't loose track of how much RAM we've used.
     * 
     * @param Size 
     */
    void DeallocateTextureRAMFromBudget(unsigned long Size);
    
    /**
     * @brief Updates the totals in the systemutils' renderer settings struct.
     * 
     */
    void UpdateTotals();

};