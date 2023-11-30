//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the simulation engine code.
    Additional Notes: None
    Date Created: 2023-11-04
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <assert.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <vsg/all.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/VecTools.h>





namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief Simple helpers that convert vector formats.
 * 
 * @param _Source 
 * @return vsg::vec3 
 */
vsg::vec3 ConvertVector(Geometries::Vec3D _Source);
Geometries::Vec3D ConvertVector(vsg::vec3 _Source);

/**
 * @brief Convert xyz radians to degrees.
 * 
 * @param _Source 
 * @return vsg::vec3 
 */
vsg::vec3 RadsToDeg(vsg::vec3 _Source);

/**
 * @brief Convert xyz degrees to radians.
 * 
 * @param _Source 
 * @return vsg::vec3 
 */
vsg::vec3 DegToRads(vsg::vec3 _Source);


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG