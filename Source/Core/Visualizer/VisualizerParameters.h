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
#include <cstdint>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <vsg/maths/vec3.h>

// Internal Libraries (BG convention: use <> instead of "")

namespace BG {
namespace NES {
namespace Simulator {

enum VisualizerState { VISUALIZER_NONE, VISUALIZER_REQUESTED, VISUALIZER_WORKING, VISUALIZER_DONE };

/**
 * @brief This struct defines a bunch of parameters to be used by the VisualizeSimulation function.
 * It's just there to make it easier to call that function so we don't have a million params.
 *
 */
struct VisualizerParameters {

    // Note that these should be the same length

    std::vector<float> FOVList_deg; /**The desired field of view in degrees in a list for all requsted images*/

    std::vector<vsg::dvec3> CameraPositionList_um;       /**List of locations of the camera in world-space coordinates*/
    std::vector<vsg::dvec3> CameraLookAtPositionList_um; /**List of locations of where the camera is looking towards in world-space coordinates*/

    int ImageWidth_px;  /**Target image width in pixels*/
    int ImageHeight_px; /**Target image height in pixels*/

    int LastImageNumber = 0; /**Number of last image - to number files*/

    VisualizerState State = VISUALIZER_NONE; /**Sets the state of this visualizer task*/

    bool VisualizeElectrodes = false; /**Enable or disable rendering of any placed electrodes in the sample*/

    std::vector<std::string> FileHandles; /**List of file handles for completed images*/
};

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
