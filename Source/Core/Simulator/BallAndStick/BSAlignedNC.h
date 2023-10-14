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

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/NeuralCircuit.h>

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

    //! Neurons in the neural circuit.
    std::unordered_map<std::string, std::shared_ptr<CoreStructs::Neuron>> Cells;

    int NumCells = 2; //! Number of cells

    //! Constructors
    BSAlignedNC(int _ID);
    BSAlignedNC(int _ID, int _NumCells);

    //! Initializes the neurons in the neural circuit.
    void InitCells(std::shared_ptr<Geometries::Geometry> domain);

    //! Returns all neurons in the neural circuit.
    std::vector<std::shared_ptr<BSNeuron>> GetNeurons();

    //! Returns all neurons in the neural circuit with specified IDs.
    std::vector<std::shared_ptr<BSNeuron>>
    GetNeuronsByID(std::vector<int> IDList);

    //! Returns the geometric centers of all neurons in the neural circuit.
    std::vector<Geometries::Vec3D> GetCellCenters();

    //! Sets the weights of pairs of synaptic connections between neurons
    //! in the neural circuit.
    void SetWeight(std::tuple<int, int> fromTo, std::string method);

    //! Sets synapse weights in the neurons according to specified methods.
    void Encode(std::vector<std::tuple<int, int>> patternSet,
                std::string encodingMethod, std::string synapseWeightMethod);

    //! Attach direct stimulation to specified neurons in neural circuit.
    void AttachDirectStim(std::vector<std::tuple<float, int>> ListOfStims);
    //! Set parameters of the distribution of the spontaneous activity
    //! in the neurons in the neural circuit.
    void SetSpontaneousActivity(std::vector<std::tuple<float, float, int>>);

    //! Updates the membrane potentials in the neurons in the circuit.
    void Update(float t_ms, bool recording);

    //! Returns recorded data from all neurons.
    std::unordered_map<std::string, NeuronRecording> GetRecording();
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
