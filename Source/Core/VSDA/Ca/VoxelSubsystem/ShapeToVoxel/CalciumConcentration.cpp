//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/Ca/VoxelSubsystem/ShapeToVoxel/CalciumConcentration.h>
#include <VSDA/Ca/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>
#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Structs/CalciumImaging.h>


namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {
namespace VoxelArrayGenerator {

/**
 * Some notes by Randal:
 * 
 * - I am implementing this function as requested, even though I have some doubts
 *   as to whether it is necessary or more efficient than just doing what I do
 *   within this function without storing the extra vector of vectors first and
 *   then pulling from that. After all, all the necessary data is already buffered
 *   in the neurons that this funtion obtains the data from.
 * - Alternatively, to get the calcium concentration of a particular component at
 *   a particular Ca sample time index. See the ComponentSampledCalciumConcentration()
 *   function below. That does precisely as many reads with exactly as many indirections
 *   and avoids building the extra vector of vectors buffer.
 *   Of course, that would be more CPU-taxing if you can't deal with all the voxels of
 *   one component at a time. Anyway, proving both methods for convenience.
 */
bool CalculateCalciumConcentrations(BG::Common::Logger::LoggingSystem *_Logger, Simulator::Simulation* _Simulation, std::vector<std::vector<float>>* _Data) {
	// Walking through the list of components as stored in Simulation:
	for (unsigned int component_id = 0; component_id < _Simulation->BSCompartments.size(); component_id++) {
		// Getting the corresponding neuron by component ID:
		auto neuron_ptr = _Simulation->FindNeuronByCompartment(component_id);

		// Ensure that the pointer isn't null and explodes
		if (neuron_ptr == nullptr) {
			_Logger->Log("Failed To Find Neuron For Compartment '" + std::to_string(component_id) + "'", 6);
			_Data->push_back(std::vector<float>());
			continue;
		}

		// Get the list of Calcium concentrations cached at that neuron and copy it into the vector of vectors:
		std::vector<float> CASamples = static_cast<Simulator::BallAndStick::BSNeuron*>(neuron_ptr)->CaSamples;
		_Data->emplace_back(CASamples);
	}
	// Ensure that all timestep lists by compartment index have at least one sample in them, otherwise we failed.
	for (size_t i = 0; i < _Data->size(); i++) {
		if ((*_Data)[i].size() < 1) {
			_Logger->Log("Failed To Get Calcium Concentrations For Neuron With ID '" + std::to_string(i) + "', Has No Timesteps", 7);
			return false;
		}
	}
	return true;
}

float ComponentSampledCalciumConcentration(Simulator::Simulation* _Simulation, int _ComponentID, size_t _SampleIdx) {
	return static_cast<Simulator::BallAndStick::BSNeuron*>(_Simulation->FindNeuronByCompartment(_ComponentID))->CaSamples[_SampleIdx];
}

/**
 * Some notes by Randal:
 * 
 * - I'm not sure that I understand what this function does, because it provides just a single float pointer
 *   in which to return time data, but it does not specify anything that would indicate which time to return.
 * - Hence, I will modify this function a little bit in an alternative below where I assume that what you
 *   meant was that you wanted to know the simulation time-point of a specific Ca sample by its _SampleIdx.
 */
bool GetCalciumConcentrationTimestep(BG::Common::Logger::LoggingSystem *_Logger, Simulator::Simulation* _Simulation, float* _Timestep){
	// *** Don't know how to make this, please see alternative function below where I make some assumptions about what was intended!
	return true;
}

float SampledCalciumConcentrationTime_ms(Simulator::Simulation* _Simulation, size_t _SampleIdx) {
	return _Simulation->CaData_.CaImaging.TRecorded_ms.at(_SampleIdx);
}

}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG