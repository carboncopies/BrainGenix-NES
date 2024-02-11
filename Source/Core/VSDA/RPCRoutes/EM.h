//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the interface for the slice generator, which creates voxel representations of a given simulation.
    Additional Notes: None
    Date Created: 2024-01-19
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

// Internal Libraries (BG convention: use <> instead of "")

#include <VSDA/VoxelSubsystem/Structs/MicroscopeParameters.h>
#include <VSDA/VoxelSubsystem/Structs/ScanRegion.h>

#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {

// Forward declaration:
struct Simulation;

// Target Process: Simulation -> Voxels -> Meshes -> Images
// Firstly, we take a simulation pointer, build a voxel array from it
// Then, we slice the voxel array one slice at a time, and build a mesh from it
// Then the mesh is used to generate an image

/**
 * @brief Helper function for VSDA subsystem.
 * Will initialize the VSDA struct within the simulation struct - for further use later.
 * 
 * @param _Logger Pointer to logging system instance
 * @param _Sim Pointer to simulation struct that we can modify with this state
 * 
 * @return true On Success
 * @return false On Error
 */
bool VSDA_EM_Initialize(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim);

/**
 * @brief Setup the virtual microscope on a given simulation with the given parameters.
 * 
 * @param _Logger Pointer to logging system instance.
 * @param _Sim Pointer to simulation instance which we will modify with our new data.
 * @param _Params Microscope parameter struct used to update the VSDAData_ struct in the sim.
 * 
 * @return true On Success
 * @return false On Error
 */
bool VSDA_EM_SetupMicroscope(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters _Params);

/**
 * @brief Defines the region that we'd like to scan with the current render operation.
 * Currently, we only support rectangular regions defined by two points in space. That's it for now unfortunately.
 * 
 * @param _Logger Pointer to logging system interface.
 * @param _Sim Pointer to simulation instance to be configured by this VSDA operation.
 * @param _ScanRegion Instance of the BoundingBox struct with the needed values populated.
 * @param _RegionID Pointer to int, which will contain the scan region id returned, if any. Check bool Result to indicate if this will be set or not.
 * 
 * @return true On Success
 * @return false On Error
 */
bool VSDA_EM_DefineScanRegion(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, ScanRegion _ScanRegion, int* _RegionID);

/**
 * @brief Sets the enum flag in the simulation that a render operation has been requested.
 * Probably should also check to ensure that all needed data has been provided before queuing this operation but we can add that later.
 * 
 * @param _Logger Pointer to logging system interface.
 * @param _Sim Pointer to simulation instance to be configured by this instance.
 * @param _RegionID Valid index of region to be rendered in this call.
 * 
 * @return true On Success
 * @return false On Error
 */
bool VSDA_EM_QueueRenderOperation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, int _RegionID);




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
