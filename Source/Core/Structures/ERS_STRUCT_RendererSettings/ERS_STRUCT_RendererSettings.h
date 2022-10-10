// ToDO: then add to project struct, then update project loader/writer with this info. Then check trello board for other related tasks like live ediitng.


//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_ENUM_ShadowFilteringType.h>
#include <ERS_ENUM_ShadowUpdateMode.h>


/**
 * @brief Struct containing renderer settings.
 * 
 */
struct ERS_STRUCT_RendererSettings {

    // Shadow Map Settings
    int ShadowMapX_ = 2048;
    int ShadowMapY_ = 2048;

    // Set Shadow Update Settings
    ERS::Renderer::ShadowFilteringType ShadowFilteringType_ = ERS::Renderer::ERS_SHADOW_FILTERING_POISSON_SAMPLING;
    ERS::Renderer::ShadowUpdateMode ShadowUpdateMode_ = ERS::Renderer::ERS_SHADOW_UPDATE_MODE_DISTANCE_PRIORITIZED;
    int MaxShadowUpdatesPerFrame_ = 5;
    int ShadowFilterKernelSize_ = 2;
    
    unsigned long long VRAMBudget_ = 0;
    unsigned long long RAMBudget_ = 0;

    unsigned long long CurrentVRAMUsage_ = 0;
    unsigned long long CurrentRAMUsage_ = 0;

    // Low Resource Thresholds
    unsigned long long int WarningLowRAMBytes    = 4294967296;
    unsigned long long int CriticalLowRAMBytes   = 1073741824;
    unsigned long long int FatalLowRAMBytes      = 268435456;
    unsigned long long int TerminateLowRAMBytes  = 67108864;

    unsigned long long int WarningLowVRAMBytes   = 1073741824;
    unsigned long long int CriticalLowVRAMBytes  = 536870912;
    unsigned long long int FatalLowVRAMBytes     = 268435456;
    unsigned long long int TerminateLowVRAMBytes = 67108864;
    
};