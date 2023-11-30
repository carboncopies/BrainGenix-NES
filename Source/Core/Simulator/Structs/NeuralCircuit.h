//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the NeuralCircuit abstract struct.
    Additional Notes: None
    Date Created: 2023-10-14
*/

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Structs/Neuron.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

/**
 * @brief This struct provides the base struct for all neural circuits.
 *
 */
struct NeuralCircuit {

    int ID; /**ID of the neural circuit */

    //! Neurons in the neural circuit.
    std::unordered_map<std::string, std::shared_ptr<Neuron>> Cells;

    //! Initializes the neurons in the neural circuit.
    virtual void InitCells(std::shared_ptr<Geometries::Geometry> domain) = 0;

    //! Returns all neurons in the neural circuit.
    virtual std::vector<std::shared_ptr<Neuron>> GetNeurons() = 0;

    //! Returns all neurons in the neural circuit with specified IDs.
    virtual std::vector<std::shared_ptr<Neuron>>
    GetNeuronsByIDs(std::vector<size_t> IDList) = 0;
};

//! CircuitRecording is an unordered map containing data from simulation from
//! all neurons in a neural circuit.
typedef std::unordered_map<std::string, CoreStructs::NeuronRecording>
    CircuitRecording;

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
