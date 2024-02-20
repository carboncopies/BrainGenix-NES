
//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the image struct, used to provide work to the encoder pool.
    Additional Notes: None
    Date Created: 2024-01-20
    Author(s): Thomas Liao


    Copyright (C) 2024  Thomas Liao

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


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/Ca/VoxelSubsystem/Structs/CaVoxelArray.h>
#include <Simulator/Geometries/GeometryCollection.h>


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {
namespace VoxelArrayGenerator {



/**
 * @brief Rasterizes the given box struct, writes it into the voxelarray in question given the scale set.
 * 
 * @param _Array 
 * @param _Box 
 * @param _VoxelScale 
 * @return true 
 * @return false 
 */
bool FillBox(VoxelArray* _Array, Simulator::Geometries::Box* _Box, size_t _CompartmentID, WorldInfo& _WorldInfo);

/**
 * @brief Rasterize the given cylinder struct, and writes it into the given voxelarray at the given scale.
 * 
 * @param _Array 
 * @param _Cylinder 
 * @param _VoxelScale 
 * @return true 
 * @return false 
 */
bool FillCylinder(VoxelArray* _Array, Simulator::Geometries::Cylinder* _Cylinder, size_t _CompartmentID, WorldInfo& _WorldInfo);

/**
 * @brief Uses a generic ispointinshape function to write an object into the voxelarray.
 * Some objects don't support this (such as boxes and cylinders).
 * 
 * @param _Array 
 * @param _Shape 
 * @param _VoxelScale 
 * @return true 
 * @return false 
 */
bool FillShape(VoxelArray* _Array, Simulator::Geometries::Geometry* _Shape, size_t _CompartmentID, WorldInfo& _WorldInfo);

/**
 * @brief Simply fills everything in the bounding box.
 * 
 * @param _Array 
 * @param _BB 
 * @param _VoxelScale 
 * @return true 
 * @return false 
 */
bool FillBoundingBox(VoxelArray* _Array, Simulator::BoundingBox* _BB, float _VoxelScale);

/**
 * @brief Generates a border frame around the region.
 * 
 * @param _Array 
 * @return true 
 * @return false 
 */
bool CreateVoxelArrayBorderFrame(VoxelArray* _Array);




}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
