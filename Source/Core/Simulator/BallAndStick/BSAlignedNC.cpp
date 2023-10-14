#include <Simulator/BallAndStick/BSAlignedNC.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

//! Constructors
BSAlignedNC::BSAlignedNC(int _ID) { this->ID = _ID; };

BSAlignedNC::BSAlignedNC(int _ID, int _NumCells) : NumCells(_NumCells) {
    this->ID = _ID;
};

//! Initializes the neurons in the neural circuit.
void BSAlignedNC::InitCells(std::shared_ptr<Geometries::Geometry> domain) {

    return;
};

//! Returns all neurons in the neural circuit.
std::vector<std::shared_ptr<BSNeuron>> BSAlignedNC::GetNeurons() {
    std::vector<std::shared_ptr<BSNeuron>> neurons;
    return neurons;
};

//! Returns all neurons in the neural circuit with specified IDs.
std::vector<std::shared_ptr<BSNeuron>>
BSAlignedNC::GetNeuronsByID(std::vector<int> IDList) {
    std::vector<std::shared_ptr<BSNeuron>> neurons;
    return neurons;
};

//! Returns the geometric centers of all neurons in the neural circuit.
std::vector<Geometries::Vec3D> BSAlignedNC::GetCellCenters() {
    std::vector<Geometries::Vec3D> cellCenters;
    return cellCenters;
};

//! Sets the weights of pairs of synaptic connections between neurons
//! in the neural circuit.
void BSAlignedNC::SetWeight(std::tuple<int, int> fromTo, std::string method) {
    return;
};

//! Sets synapse weights in the neurons according to specified methods.
void BSAlignedNC::Encode(std::vector<std::tuple<int, int>> patternSet,
                         std::string encodingMethod,
                         std::string synapseWeightMethod) {
    return;
};

//! Attach direct stimulation to specified neurons in neural circuit.
void BSAlignedNC::AttachDirectStim(
    std::vector<std::tuple<float, int>> ListOfStims) {
    return;
};

//! Set parameters of the distribution of the spontaneous activity
//! in the neurons in the neural circuit.
void BSAlignedNC::SetSpontaneousActivity(
    std::vector<std::tuple<float, float, int>>) {
    return;
};

//! Updates the membrane potentials in the neurons in the circuit.
void BSAlignedNC::Update(float t_ms, bool recording) { return; };

//! Returns recorded data from all neurons.
std::unordered_map<std::string, NeuronRecording> BSAlignedNC::GetRecording() {
    std::unordered_map<std::string, NeuronRecording> recording;
    return recording;
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
