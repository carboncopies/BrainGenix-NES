
//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the scan region struct.
    Additional Notes: None
    Date Created: 2024-02-20
    Author(s): Thomas Liao, Randal Koene


    Copyright (C) 2024  Thomas Liao, Randal Koene

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
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")

namespace BG {
namespace NES {
namespace VSDA {

/**
 * @brief Simple helper struct that provides some information about the world to functions used for rasterization.
 * 
 */
struct WorldInfo {

    float WorldRotationOffsetX_rad;
    float WorldRotationOffsetY_rad;
    float WorldRotationOffsetZ_rad;

    float VoxelScale_um; /**Size of each voxel in microns*/

};


}; // namespace Simulator
}; // namespace NES
}; // namespace BG
