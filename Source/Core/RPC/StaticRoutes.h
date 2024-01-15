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
 * @brief Static RPC function used to get the current version of the API Spec.
 *
 * This function retrieves the current version of the API specification supported by this instance and returns it as a string.
 *
 * @return std::string - The current version of the API Spec.
 */
std::string GetAPIVersion();


/**
 * @brief Diagnostic function that echoes back the provided string.
 *
 * This function takes a string as input and returns the same string, serving as a diagnostic or testing tool.
 *
 * @param _Data - The input string to be echoed.
 * @return std::string - The echoed string.
 */
std::string Echo(std::string _Data);


}; // Close Namespace API
}; // Close Namespace NES
}; // Close Namespace BG