//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Date Created: 2023-10-30
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
#include <iostream>
#include <assert.h>


// Third-Party Libraries (BG convention: use <> instead of "")

#include <vsg/all.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/Math/MathHelpers.h>


namespace BG {
namespace NES {
namespace Renderer {


/**
 * @brief Helper function that creates the camera's perspective matrix using the given parameters.
 * 
 * @param _FOV Field of view of the camera in degrees.
 * @param _AspectRatio Horizontal render area / vertical area.
 * @param _NearClip Minimum distance stuff has to be from the camera before it gets hidden (anything closer will be omitted).
 * @param _FarClip Maximum distance stuff can be from the camera before we don't draw it anymore. (anything farther will be omitted).
 * @return vsg::ref_ptr<vsg::ProjectionMatrix> 
 */
vsg::ref_ptr<vsg::ProjectionMatrix> CreatePerspectiveMatrix(float _FOV, double _AspectRatio, double _NearClip = 0.1, double _FarClip = 100.0);


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG