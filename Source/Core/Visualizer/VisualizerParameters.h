//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines paremters for the visualizer.
    Additional Notes: None
    Date Created: 2024-02-07
    Author(s): Thomas Liao


    Copyright (C) 2023  Thomas Liao

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once



// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
#include <vsg/maths/vec3.h>

// Internal Libraries (BG convention: use <> instead of "")


namespace BG {
namespace NES {
namespace Simulator {


/**
 * @brief This struct defines a bunch of parameters to be used by the VisualizeSimulation function.
 * It's just there to make it easier to call that function so we don't have a million params.
 * 
 */
struct VisualizerParameters {

    float FOV_deg; /**The desired field of view in degrees*/

    vsg::dvec3 CameraPosition_um; /**Location of the camera in world-space coordinates*/
    vsg::dvec3 CameraLookAtPosition_um; /**Location of where the camera is looking towards in world-space coordinates*/

    int ImageWidth_px; /**Target image width in pixels*/
    int ImageHeight_px; /**Target image height in pixels*/

};



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
