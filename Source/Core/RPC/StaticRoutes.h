//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides some basic static routes for NES.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Version.h>


namespace BG {
namespace NES {
namespace API {

/**
 * @brief Static RPC function used to get the current version of the API Spec and return it.
 * 
 * @return std::string 
 */
std::string GetAPIVersion();


/**
 * @brief Diagnostic function that simply echoes back the string provided.
 * 
 * @param _Data 
 * @return std::string 
 */
std::string Echo(std::string _Data);


}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG