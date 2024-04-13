//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description:

    Additional Notes:
        None

    Date Created:
        2024-03-14
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <bbp/sonata/edges.h>
#include <bbp/sonata/nodes.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Neuron.h>

namespace BG {
namespace NES {
namespace IO {
class SonataLoader {
  public:
    //! Nodes loaded from a H5 file containing node population information.
    std::shared_ptr<bbp::sonata::NodeStorage> nodes;
    //! Edges loaded from a H5 file containing edge population information.
    std::shared_ptr<bbp::sonata::EdgeStorage> edges;

    //! Constructor
    SonataLoader(const std::string &_nodeFilePath, const std::string &_edgeFilePath);

    //! Get all neurons
    std::vector<std::shared_ptr<Simulator::CoreStructs::Neuron>> GetAllNeurons();
};
} // namespace IO
} // namespace NES
} // namespace BG
