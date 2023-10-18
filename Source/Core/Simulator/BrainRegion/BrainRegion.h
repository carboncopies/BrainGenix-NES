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
namespace BrainRegion {

/**
 * @brief This class defines the characteristics of a brain region, such as
 * geometric shape and physiological content.
 *
 */
class BrainRegion {
  public:
    size_t ID;
    std::shared_ptr<Geometries::Geometry> Shape{};
    std::shared_ptr<CoreStructs::NeuralCircuit> Content{};

    //! Constructors
    BrainRegion();

    virtual void Show(float lineWidth) = 0;
};

}; // namespace BrainRegion
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
