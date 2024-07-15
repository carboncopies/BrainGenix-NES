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

#include <VSDA/EM/VoxelSubsystem/Structs/MicroscopeParameters.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>


#include <VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.h>

#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {




/**
 * @brief Generate a sample tear on the given array, at the given ZHeight.
 * 
 * @param _Logger 
 * @param _Params 
 * @param _Array 
 * @param _ZHeight 
 * @return true 
 * @return false 
 */
int GenerateTear(BG::Common::Logger::LoggingSystem* _Logger, std::vector<std::unique_ptr<VoxelArrayGenerator::Task>>& _TaskList, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool, ScanRegion _Region, MicroscopeParameters* _Params, VoxelArray* _Array, VSDA::WorldInfo _Info, int _ZHeight, int _Seed);




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
