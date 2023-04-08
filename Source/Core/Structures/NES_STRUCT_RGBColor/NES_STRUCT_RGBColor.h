//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <memory.h>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")


/**
 * @brief This class stores an rgb color value.
 * 
 */
struct NES_STRUCT_RGBColor {

    int Red; /**<Red Component*/
    int Green; /**<Green Component*/
    int Blue; /**<Blue Component*/

};