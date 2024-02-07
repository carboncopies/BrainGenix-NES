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
#include <Simulator/Geometries/GeometryCollection.h>
#include <Simulator/Structs/Neuron.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

/*
SOME NOTES ON THINGS TO FIX HERE (by Randal):

- We want to be able to work with mixed types, so NeuralCircuit and the general
  map of neurons of base class Neuron should be the holder of pointers to
  neuron objects (not some derived class such as BSAlignedNC).
- While the use of virtual member functions can allow duplicate member functions
  for derived classes there should be no duplicate variables (e.g. the
  Cells map). Push these as low as possible, i.e. towards the base class, except
  where that does not make sense (e.g. where the variables are specific to one
  derived class).

Note that the above is also how the Python prototypes now work.

- We also have a little bit of a confusing issue where a decision needs to be
  made: In both the model code and the BG_API.py test, a DAC is set up to
  put input into a compartment (e.g. a soma). But, running a simulation
  depends entirely on carrying out updates on a circuit and from there
  neurons and from there compartments. There is no way to reach compartments
  directly for updates during a simulation. We need to decide: Do we keep it
  that way and ensure that neurons and circuits are always created or do we
  add a way to run compartments directly without caring about the higher
  level descriptions. A couple of thoughts in favor of either: a) It is
  highly likely that we will never create compartments without them belonging
  to a neuron and a circuit. b) It could be that just updating compartments
  is all that is needed for simulation runs and that higher level composites
  can be ignored entirely.
  For now, we should probably pick running from the circuit and neuron, because
  update functions such as possible spontaneous activity are only defined at
  the neuron and not the compartment in the Python prototypes.
*/

/**
 * @brief This struct provides the base struct for all neural circuits.
 *
 */
struct NeuralCircuit {

    int ID; /**ID of the neural circuit */

    Geometries::GeometryCollection * Collection_ptr = nullptr; // Obtained from Simulation.

    //! Neurons in the neural circuit.
    std::unordered_map<std::string, std::shared_ptr<Neuron>> Cells;

    //! Initializes the neurons in the neural circuit.
    virtual void InitCells(Geometries::Box * domain) = 0;

    //! Returns the number of neuron in the neural circuit.
    virtual size_t GetNumberOfNeurons() { return Cells.size(); }

    //! Returns all neurons in the neural circuit.
    virtual std::vector<std::shared_ptr<Neuron>> GetNeurons() = 0;

    //! Returns all neurons in the neural circuit with specified IDs.
    virtual std::vector<std::shared_ptr<Neuron>>
    GetNeuronsByIDs(std::vector<size_t> IDList) = 0;

    virtual nlohmann::json GetRecordingJSON() const;

    NeuralCircuit(Geometries::GeometryCollection * _Collection_ptr): Collection_ptr(_Collection_ptr) {}
};

//! CircuitRecording is an unordered map containing data from simulation from
//! all neurons in a neural circuit.
typedef std::unordered_map<std::string, CoreStructs::NeuronRecording>
    CircuitRecording;

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
