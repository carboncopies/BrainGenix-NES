//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <memory.h>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>

#include <ERS_Version.h>
#include <ERS_CompilePlatformInfo.h>
#include <ERS_CompileTimeStamp.h>


/**
 * @brief Logs the system information as defined during compilation (version, target os, etc.)
 * 
 */
void ERS_LogSystemInfo(ERS_LoggingSystem* Logger);
