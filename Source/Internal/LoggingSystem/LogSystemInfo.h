//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//
#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <memory.h>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <LoggingSystem.h>

#include <Version.h>
#include <CompilePlatformInfo.h>
#include <CompileTimeStamp.h>



namespace BrainGenix {
namespace NES {
namespace Module {


/**
 * @brief Logs the system information as defined during compilation (version, target os, etc.)
 * 
 */
void LogSystemInfo(LoggingSystem* Logger);


}
}
}