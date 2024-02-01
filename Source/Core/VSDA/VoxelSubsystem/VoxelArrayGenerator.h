//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the interface for the slice generator, which creates voxel representations of a given simulation.
    Additional Notes: None
    Date Created: 2024-01-19
    Author(s): Thomas Liao, Randal Koene


    Copyright (C) 2023  Thomas Liao, Randal Koene

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
#include <Simulator/Structs/Simulation.h>

#include <VSDA/VoxelSubsystem/Structs/MicroscopeParameters.h>
#include <VSDA/VoxelSubsystem/Structs/VoxelArray.h>


#include <VSDA/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.h>

#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {




/**
 * @brief Create a Voxel Array From Simulation object
 * Generates a voxel array at pointer _Array, within the region defined in _Region, from the simulation _Sim.
 * 
 * @param _Logger Pointer to logging system instance
 * @param _Sim Pointer to simulation that data is to be generated from
 * @param _Region Pointer to region in that simulation where we'll be generating an array
 * @param _Array Pointer to array to be populated.
 * @return true On success
 * @return false On failure (eg: out of memory, out of bounds, etc.)
 */
bool CreateVoxelArrayFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters* _Params, VoxelArray* _Array);
bool CreateVoxelArrayFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters* _Params, VoxelArray* _Array, ScanRegion _Region, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool);




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
