//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the interface which connects the NES routes for VSDA to the API.
    Additional Notes: None
    Date Created: 2023-12-02
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
#include <vector>
#include <memory>
#include <iostream>
#include <assert.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>

#include <VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.h>
#include <VSDA/EM/VoxelSubsystem/VoxelArrayRenderer.h>
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h>
#include <VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.h>
#include <VSDA/EM/VoxelSubsystem/Structs/SubRegion.h>

#include <BG/Common/Logger/Logger.h>




namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {


/**
 * @brief Renders an EM subregion component.
 * Takes a pointer to the region in question which needs rendering.
 * 
 * @param _Logger 
 * @param _SubRegion 
 * @param _ImageProcessorPool 
 * @param _GeneratorPool 
 * @return true 
 * @return false 
 */
bool EMRenderSubRegion(BG::Common::Logger::LoggingSystem* _Logger, SubRegion* _SubRegion, ImageProcessorPool* _ImageProcessorPool, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool);




}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
