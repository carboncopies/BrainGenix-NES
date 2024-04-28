#include "IO/SonataLoader.h"

namespace BG {
namespace NES {
namespace IO {
SonataLoader::SonataLoader(const std::string &_nodeFilePath, const std::string &_edgeFilePath) {
    nodes = std::make_shared<bbp::sonata::NodeStorage>(_nodeFilePath);
    edges = std::make_shared<bbp::sonata::EdgeStorage>(_edgeFilePath);

    std::vector<std::tuple<size_t, size_t>> allNodePairs;
    std::vector<size_t> allNodeIds;

    std::cout << "Node populations:\n";
    for (const auto &name : nodes->populationNames()) {
        _allNodes[name] = nodes->openPopulation(name);
        std::cout << name << ": " << _allNodes[name]->size() << "\n";
    }

    std::cout << "Edge populations:\n";
    for (const auto &name : edges->populationNames()) {
        _allEdges[name] = edges->openPopulation(name);
        std::cout << name << ": " << _allEdges[name]->size() << "\n";
    }

    std::cout << "Node attribute names:\n";
    for (const auto &[k, v] : _allNodes) {
        std::cout << k << ": ";
        for (const auto &attr : v->attributeNames()) {
            std::cout << attr << " ";
        }
        std::cout << "\n";

        auto allSelectedNodes = v->selectAll().flatten();

        allNodeIds.insert(allNodeIds.end(), allSelectedNodes.begin(), allSelectedNodes.end());
    }

    std::cout << "Edge attribute names:\n";
    for (const auto &[k, v] : _allEdges) {
        std::cout << k << ": ";
        for (const auto &attr : v->attributeNames()) {
            std::cout << attr << " ";
        }
        std::cout << "\n";
        auto allSelected = v->selectAll();
        auto srcNodeIDs = v->sourceNodeIDs(allSelected);
        auto tgtNodeIDs = v->targetNodeIDs(allSelected);

        for (size_t i = 0; i < srcNodeIDs.size(); ++i) {
            allNodePairs.emplace_back(std::make_tuple(srcNodeIDs[i], tgtNodeIDs[i]));
        }
    }
}

std::vector<std::shared_ptr<Simulator::CoreStructs::Neuron>> SonataLoader::GetAllNeurons(Simulator::Simulation &_Sim) {
    std::vector<std::shared_ptr<Simulator::CoreStructs::Neuron>> allNeurons;
    std::vector<int> somaCompartmentIDs = {0, 1, 2, 4};
    std::vector<int> dendriteCompartmentIDs = {3, 5, 7, 9};
    std::vector<int> axonCompartmentIDs = {6, 8};

    for (const auto &[k, v] : _allNodes) {
        auto allSelectedNodes = v->selectAll().flatten();
        for (auto it = allSelectedNodes.begin(); it != allSelectedNodes.end(); ++it) {
            auto neuronStruct = Simulator::CoreStructs::SCNeuronStruct{
                Name : "dummy",
                ID : (int)*it,
                SomaCompartmentIDs : somaCompartmentIDs,
                DendriteCompartmentIDs : dendriteCompartmentIDs,
                AxonCompartmentIDs : axonCompartmentIDs,

                MembranePotential_mV : 20.0,
                RestingPotential_mV : 20.0,
                SpikeThreshold_mV : 20.0,
                DecayTime_ms : 20.0,
                AfterHyperpolarizationAmplitude_mV : 20.0,
                PostsynapticPotentialRiseTime_ms : 20.0,
                PostsynapticPotentialDecayTime_ms : 20.0,
                PostsynapticPotentialAmplitude_nA : 20.0
            };

            allNeurons.emplace_back(std::make_shared<Simulator::SCNeuron>(neuronStruct, _Sim));
        }
    }
    return allNeurons;
}
} // namespace IO
} // namespace NES
} // namespace BG
