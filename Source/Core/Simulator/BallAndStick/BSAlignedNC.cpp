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
    std::vector<std::vector<float>> domainBounds;
    std::shared_ptr<Geometries::Sphere> soma = nullptr;
    std::shared_ptr<Geometries::Cylinder> axon = nullptr;
    auto boxDomain = std::dynamic_pointer_cast<Geometries::Box>(domain);

    assert(boxDomain);

    for (size_t i = 0; i < this->NumCells; ++i) {
        domainBounds = boxDomain->EqualSliceBounds(this->NumCells, i);
        soma = CreateBSSoma(domainBounds, Align::ALIGN_LEFT);
        axon = CreateBSAxon(domainBounds, Align::ALIGN_RIGHT, soma->Radius_um);
        this->Cells[std::to_string(i)] =
            std::make_shared<BSNeuron>(i, soma, axon);
    }
};

//! Returns all neurons in the neural circuit.
std::vector<std::shared_ptr<CoreStructs::Neuron>> BSAlignedNC::GetNeurons() {
    std::vector<std::shared_ptr<CoreStructs::Neuron>> neurons;

    for (auto &[cellID, cell] : this->Cells)
        neurons.push_back(cell);

    return neurons;
};

//! Returns all neurons in the neural circuit with specified IDs.
std::vector<std::shared_ptr<CoreStructs::Neuron>>
BSAlignedNC::GetNeuronsByIDs(std::vector<size_t> IDList) {
    std::vector<std::shared_ptr<CoreStructs::Neuron>> neurons;

    for (const size_t id : IDList)
        neurons.emplace_back(this->Cells.at(std::to_string(id)));
    return neurons;
};

//! Returns the geometric centers of all neurons in the neural circuit.
std::vector<Geometries::Vec3D> BSAlignedNC::GetCellCenters() {
    std::vector<Geometries::Vec3D> cellCenters;

    for (const auto &[cellID, cell] : this->Cells)
        cellCenters.push_back(cell->GetCellCenter());
    return cellCenters;
};

//! Sets the weights of synaptic connections between pairs of neurons
//! in the neural circuit.
void BSAlignedNC::SetWeight(size_t from, size_t to, SetWeightMethod method) {
    std::shared_ptr<BSNeuron> sourceCell, targetCell;

    auto itTo = this->Cells.find(std::to_string(to));
    auto itFrom = this->Cells.find(std::to_string(from));

    if (itTo == this->Cells.end())
        throw std::invalid_argument("Unknown target cell.");
    if (itFrom == this->Cells.end())
        throw std::invalid_argument("Unknown source cell.");

    switch (method) {
    case SetWeightMethod::BINARY:
        targetCell = std::dynamic_pointer_cast<BSNeuron>(itTo->second);
        sourceCell = std::dynamic_pointer_cast<BSNeuron>(itFrom->second);

        // source and weight
        targetCell->ReceptorDataVec.push_back(std::make_tuple(sourceCell, 1.0));
        targetCell->Morphology["receptor"] =
            CreateBSReceptor(sourceCell->Morphology["soma"]->Center_um);
        break;
    default:
        break;
    }
    return;
};

//! Sets synapse weights in the neurons according to specified methods.
void BSAlignedNC::Encode(std::vector<std::tuple<size_t, size_t>> patternSet,
                         EncodingMethod encodingMethod,
                         SetWeightMethod synapseWeightMethod) {
    switch (encodingMethod) {
    case EncodingMethod::INSTANT:
        for (const auto &pattern : patternSet)
            this->SetWeight(std::get<0>(pattern), std::get<1>(pattern),
                            synapseWeightMethod);
        break;
    default:
        break;
    }
    return;
};

//! Attach direct stimulation to specified neurons in neural circuit.
void BSAlignedNC::AttachDirectStim(
    std::vector<std::tuple<float, size_t>> ListOfStims) {
    for (auto Stim : ListOfStims) {
        size_t cellID = std::get<1>(Stim);
        float t_ms = std::get<0>(Stim);

        auto it = this->Cells.find(std::to_string(cellID));
        if (it == this->Cells.end())
            throw std::invalid_argument("Cell not found.");
        auto cellPtr = std::dynamic_pointer_cast<BSNeuron>(it->second);
        cellPtr->AttachDirectStim(t_ms);
    }
};

//! Set parameters of the distribution of the spontaneous activity
//! in the neurons in the neural circuit.
void BSAlignedNC::SetSpontaneousActivity(
    std::vector<std::tuple<float, float, size_t>> spontSpikeSettings) {

    for (const auto &setting : spontSpikeSettings) {
        float mean = std::get<0>(setting);
        float std = std::get<1>(setting);
        size_t cellID = std::get<2>(setting);

        auto cellIt = this->Cells.find(std::to_string(cellID));

        if (cellIt == this->Cells.end())
            throw std::invalid_argument("Cell not found.");

        auto cellPtr = std::dynamic_pointer_cast<BSNeuron>(cellIt->second);
        cellPtr->SetSpontaneousActivity(mean, std);
    }
};

//! Updates the membrane potentials in the neurons in the circuit.
void BSAlignedNC::Update(float t_ms, bool recording) {
    assert(t_ms >= 0.0);
    for (auto &[cellID, cell] : this->Cells) {
        auto BSNeuronPtr = std::dynamic_pointer_cast<BSNeuron>(cell);
        BSNeuronPtr->Update(t_ms, recording);
    }
};

//! Returns recorded data from all neurons.
std::unordered_map<std::string, CoreStructs::NeuronRecording>
BSAlignedNC::GetRecording() {
    BG::NES::Simulator::CoreStructs::CircuitRecording recording;
    for (auto &[cellID, cell] : this->Cells) {
        auto BSNeuronPtr = std::dynamic_pointer_cast<BSNeuron>(cell);
        recording[cellID] = BSNeuronPtr->GetRecording();
    }

    return recording;
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
