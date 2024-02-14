
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
#include <BG/Common/Logger/Logger.h>


#include <Simulator/Structs/Simulation.h>


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {
namespace VoxelArrayGenerator {



/**
 * @brief Will enumerate the simulated compartments and build a list in the given pointer containing a list of floats for each calcium concentration by timestep.
 * 
 * Please see the example below:
 * [
 *     [1,2,3,...] <- for compartment with index 0 (also known as the CompartmentID)
 *     [12,4,453,6,...] <- for compartment with index 1 (also known as the CompartmentID)
 *     ...
 *     [blah] <- for compartment with index n (also known as the CompartmentID)
 * ]
 * 
 * In essence, this should return a list of list of concentrations where the index in that first list of lists can be matched with the CompartmentID to which it corresponds.
 * 
 * @param _Logger Pointer to logging system instance.
 * @param _Simulation Pointer to simulation instance.
 * @param _Data Pointer to vector of vectors for recorded concentrations.
 * @return true Success
 * @return false Failure
 */
bool CalculateCalciumConcentrations(BG::Common::Logger::LoggingSystem *_Logger, Simulator::Simulation* _Simulation, std::vector<std::vector<float>>* _Data);

/**
 * @brief Alternative method by which to obtain Calcium concentration of a particular component
 *        for a particular sample index (corresponding to a specific sampling time).
 * 
 * @param _Simulation Pointer to simulatin instance.
 * @param _ComponentID ID of the component. Must be a valid component ID. (*** Could add a test if it seems necessary.)
 * @param _SampleIdx Index of the sample (corresponding to a Ca sample time).
 * 
 * @return The Calcium concentration.
 */
float ComponentSampledCalciumConcentration(Simulator::Simulation* _Simulation, int _ComponentID, size_t _SampleIdx);

/**
 * @brief Return the timestep of the recorded calcium concentrations. A pair function that goes along with CalculateCalciumConcentrations.
 * 
 * @param _Logger 
 * @param _Simulation 
 * @param _Timestep 
 * @return true 
 * @return false 
 */
bool GetCalciumConcentrationTimestep(BG::Common::Logger::LoggingSystem *_Logger, Simulator::Simulation* _Simulation, float* _Timestep);

/**
 * @brief Alternative method to obtain the simulation time for a sample by sample index.
 *        (See comments in .cpp file for the reasons why.)
 * 
 * @param _Simulation Pointer to simulatin instance.
 * @param _SampleIdx Index of the sample (corresponding to a Ca sample time).
 * 
 * @return The corresponding simulation time in milliseconds.
 */
float SampledCalciumConcentrationTime_ms(Simulator::Simulation* _Simulation, size_t _SampleIdx);

}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
