#include "IO/SonataLoader.h"

namespace BG {
namespace NES {
namespace IO {
SonataLoader::SonataLoader(const std::string &_nodeFilePath,
                           const std::string &_edgeFilePath) {
    nodes = std::make_shared<bbp::sonata::NodeStorage>(_nodeFilePath);
    edges = std::make_shared<bbp::sonata::EdgeStorage>(_edgeFilePath);

    std::cout << "Node populations:\n";
    for (const auto &name : nodes->populationNames()) {
        auto pop = nodes->openPopulation(name);
        std::cout << name << ": " << pop->size() << "\n";
    }

    std::cout << "Edge populations:\n";
    for (const auto &name : edges->populationNames()) {
        auto pop = edges->openPopulation(name);
        std::cout << name << ": " << pop->size() << "\n";
    }
}
} // namespace IO
} // namespace NES
} // namespace BG
