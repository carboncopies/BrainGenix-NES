//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the image struct, used to provide work to the encoder pool.
    Additional Notes: None
    Date Created: 2024-01-19
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
#include <vector>
#include <memory>
#include <atomic>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/Ca/VoxelSubsystem/Structs/CaVoxelArray.h>
#include <Simulator/Geometries/GeometryCollection.h>

#include <VSDA/Common/Structs/WorldInfo.h>


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {
namespace VoxelArrayGenerator {


/**
 * @brief Structure that defines the work to be completed. This involves taking a pointer to the voxel array in question,
 * calculating a point cloud for the shape we've been given, and then writing that into the voxel array.
 * 
 */
struct Task {

    /**
     * @brief Index of the relevant shape from the shapes array of the simulation.
     */
    size_t                          ShapeID_;            /**Index of the relevant shape from the shapes array of the simulation.*/
    size_t                          CompartmentID_;      /**ID of the compartment that this shape belongs to (for calcium generation)*/
    // float                           VoxelResolution_um_; /**Set the resolution size of each voxel on axis in microns.*/
    VSDA::WorldInfo                 WorldInfo_;          /**World info data used for offsetting rotations, setting voxel scale, etc.*/
    Simulator::Geometries::GeometryCollection* GeometryCollection_; /**Pointer to instance of the simulation's geometry collection.*/ 
    std::atomic_bool                IsDone_ = false;     /**Indicates if this task has been processed or not.*/
    VoxelArray*                     Array_ = nullptr;    /**Pointer to the voxel array that we're writing to.*/

};

}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
