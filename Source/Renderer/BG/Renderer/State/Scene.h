//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains the NES Rendering Manager. It is responsible for the underlying initialization and ownership of the Vulkan subsystem.
    Additional Notes: None
    Date Created: 2023-10-26
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
#include <vsgXchange/all.h>


// Internal Libraries (BG convention: use <> instead of "")




namespace BG {
namespace NES {
namespace Renderer {
namespace State {



/**
 * @brief Struct that holds the data needed for a scene.
 * Contains the geometry itself as well as any associated cameras, lights, etc.
 * 
 */
struct Scene {

    vsg::Group Scene_; /**Group that contains the scene graph*/


};




}; // Close Namespace State
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG