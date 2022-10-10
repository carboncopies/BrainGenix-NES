//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_ControllerInputManager.h>



/**
 * @brief This system is responsible for getting controller input
 * 
 */
struct ERS_STRUCT_HumanInputDeviceUtils {


    std::unique_ptr<ERS_CLASS_ControllerInputManager> ControllerInputManager; /**<Pointer to ERS Controller Input Manager*/


};