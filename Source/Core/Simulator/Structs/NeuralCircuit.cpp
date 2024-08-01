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

/**
 * Return a flattened struct describing neuron indices of
 * the NeuralCircuit. This is useful for storage, among other things.
 */
std::unique_ptr<uint8_t[]> NeuralCircuit::GetFlat() const {
    NeuralCircuitStructFlatHeader header;

    header.Base = *this;
    header.FlatBufSize = sizeof(header);

    header.NeuronIDsSize = NeuronIDs.size();
    header.NeuronIDsOffset = sizeof(header);
    size_t NeuronIDsSizeOf = sizeof(decltype(NeuronIDs)::value_type)*header.NeuronIDsSize;
    header.FlatBufSize += NeuronIDsSizeOf;

    std::unique_ptr<uint8_t[]> flatbuf = std::make_unique<uint8_t[]>(header.FlatBufSize);

    memcpy(flatbuf.get(), &header, sizeof(header));
    memcpy(flatbuf.get()+header.NeuronIDsOffset, NeuronIDs.data(), NeuronIDsSizeOf);

    return flatbuf;
}

/**
 * (Re)instantiate this object from a flat representation, e.g. from
 * storage.
 */
bool NeuralCircuit::FromFlat(NeuralCircuitStructFlatHeader* header) {
    CircuitBase::operator=(header->Base);

    NeuronIDs.resize(header->NeuronIDsSize);
    memcpy(NeuronIDs.data(), ADD_BYTES_TO_POINTER(header, header->NeuronIDsOffset), header->FlatBufSize - header->NeuronIDsOffset);

    return true;
}

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
