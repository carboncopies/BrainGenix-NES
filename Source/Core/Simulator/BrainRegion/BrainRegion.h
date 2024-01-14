//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the BrainRegion class, which defines the
    characteristics of a brain region, such as geometric shape and
    physiological content.

    Additional Notes: None
    Date Created: 2023-10-18
*/

#pragma once

#include <memory>

#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Structs/NeuralCircuit.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BrainRegions {

/**
 * @brief This class defines the characteristics of a brain region, such as
 * geometric shape and physiological content.
 *
 */
class BrainRegion {
  public:
    size_t ID;
    Geometries::Geometry * Shape{nullptr}; // Regular pointers, because the objects are maintained in Simulation.Collection.
    std::shared_ptr<CoreStructs::NeuralCircuit> Content{};

    virtual void Show(float lineWidth) = 0;
};

}; // namespace BrainRegions
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
