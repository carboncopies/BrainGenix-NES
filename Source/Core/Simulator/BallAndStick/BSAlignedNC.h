//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the BSAlignedNC struct, representing
                 a ball-and-stick neural circuit.
    Additional Notes: None
    Date Created: 2023-10-14
*/

#pragma once

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <Simulator/BallAndStick/BSMorphology.h>
#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Geometries/Box.h>
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/NeuralCircuit.h>
#include <Simulator/Structs/Neuron.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

/**
 * @brief This struct provides the structure representing a ball-and-stick
 * neural circuit.
 *
 */
struct BSAlignedNC : CoreStructs::NeuralCircuit {

    //! Methods for setting the weight of synaptic connections between
    //! pairs of neurons.
    enum SetWeightMethod {
        BINARY = 0,
    };

    //! Methods for encoding weights of the cells in synaptic connections
    //! between pairs of neurons.
    enum EncodingMethod {
        INSTANT = 0,
    };

    //! Neurons in the neural circuit.
    std::unordered_map<std::string, std::shared_ptr<CoreStructs::Neuron>> Cells;

    int NumCells = 2; //! Number of cells

    //! Constructors
    BSAlignedNC(int _ID, Geometries::GeometryCollection * _Collection_ptr);
    BSAlignedNC(int _ID, int _NumCells, Geometries::GeometryCollection * _Collection_ptr);

    //! Initializes the neurons in the neural circuit.
    void InitCells(Geometries::Box * domain) override;

    //! Returns all neurons in the neural circuit.
    std::vector<std::shared_ptr<CoreStructs::Neuron>> GetNeurons() override;

    //! Returns all neurons in the neural circuit with specified IDs.
    std::vector<std::shared_ptr<CoreStructs::Neuron>>
    GetNeuronsByIDs(std::vector<size_t> IDList) override;

    //! Returns the geometric centers of all neurons in the neural circuit.
    std::vector<Geometries::Vec3D> GetCellCenters();

    //! Sets the weights of pairs of synaptic connections between neurons
    //! in the neural circuit.
    void SetWeight(size_t from, size_t to, SetWeightMethod method);

    //! Sets synapse weights in the neurons according to specified methods.
    void Encode(std::vector<std::tuple<size_t, size_t>> patternSet,
                EncodingMethod encodingMethod,
                SetWeightMethod synapseWeightMethod);

    //! Attach direct stimulation to specified neurons in neural circuit.
    void AttachDirectStim(std::vector<std::tuple<float, size_t>> ListOfStims);
    //! Set parameters of the distribution of the spontaneous activity
    //! in the neurons in the neural circuit.
    void SetSpontaneousActivity(
        std::vector<std::tuple<float, float, size_t>> spontSpikeSettings);

    //! Updates the membrane potentials in the neurons in the circuit.
    void Update(float t_ms, bool recording);

    //! Returns recorded data from all neurons.
    CoreStructs::CircuitRecording GetRecording();
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
