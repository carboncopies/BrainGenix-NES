#include "IO/SonataLoader.h"

namespace BG {
namespace NES {
namespace IO {
SonataLoader::SonataLoader(const std::string &_nodeFilePath,
                           const std::string &_edgeFilePath) {
    nodes = std::make_shared<bbp::sonata::NodeStorage>(_nodeFilePath);
    edges = std::make_shared<bbp::sonata::EdgeStorage>(_edgeFilePath);

    std::map<std::string, std::shared_ptr<bbp::sonata::NodePopulation>>
        allNodes;
    std::map<std::string, std::shared_ptr<bbp::sonata::EdgePopulation>>
        allEdges;
    std::vector<std::tuple<size_t, size_t>> allNodePairs;
    std::vector<size_t> allNodeIds;

    std::cout << "Node populations:\n";
    for (const auto &name : nodes->populationNames()) {
        allNodes[name] = nodes->openPopulation(name);
        std::cout << name << ": " << allNodes[name]->size() << "\n";
    }

    std::cout << "Edge populations:\n";
    for (const auto &name : edges->populationNames()) {
        allEdges[name] = edges->openPopulation(name);
        std::cout << name << ": " << allEdges[name]->size() << "\n";
    }

    std::cout << "Node attribute names:\n";
    for (const auto &[k, v] : allNodes) {
        std::cout << k << ": ";
        for (const auto &attr : v->attributeNames()) {
            std::cout << attr << " ";
        }
        std::cout << "\n";

        auto allSelectedNodes = v->selectAll().flatten();

        allNodeIds.insert(allNodeIds.end(), allSelectedNodes.begin(),
                          allSelectedNodes.end());

        // for (const auto &val : allSelectedNodes)
        //     std::cout << val << " ";
        // std::cout << "\n";
    }

    std::cout << "Edge attribute names:\n";
    for (const auto &[k, v] : allEdges) {
        std::cout << k << ": ";
        for (const auto &attr : v->attributeNames()) {
            std::cout << attr << " ";
        }
        std::cout << "\n";
        auto allSelected = v->selectAll();
        auto srcNodeIDs = v->sourceNodeIDs(allSelected);
        auto tgtNodeIDs = v->targetNodeIDs(allSelected);

        for (size_t i = 0; i < srcNodeIDs.size(); ++i) {
            allNodePairs.emplace_back(
                std::make_tuple(srcNodeIDs[i], tgtNodeIDs[i]));
            // std::cout << srcNodeIDs[i] << " " << tgtNodeIDs[i] << "\n";
        }
    }
}
} // namespace IO
} // namespace NES
} // namespace BG
