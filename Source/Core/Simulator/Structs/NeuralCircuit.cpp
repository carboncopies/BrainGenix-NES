//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the NeuralCircuit abstract struct.
    Additional Notes: None
    Date Created: 2024-01-07
*/

#include <Simulator/Structs/NeuralCircuit.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

nlohmann::json NeuralCircuit::GetRecordingJSON() const {
	nlohmann::json recording;

	for (const auto & [neuronID, neuron_ptr] : this->Cells) {
        assert(neuron_ptr);
        recording[neuronID] = neuron_ptr->GetRecordingJSON();
    }

	return recording;
}

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
