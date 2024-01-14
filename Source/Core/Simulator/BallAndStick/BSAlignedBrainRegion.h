//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the BSAlignedBrainRegion class, which
    defines the characteristics of a brain region, such as geometric shape and
    physiological content. It derives from the BrainRegion abstract class and
    is specifically for the ball and stick neural circuit architecture.

    Additional Notes: None
    Date Created: 2023-10-18
*/

#pragma once

#include <memory>

#include <Simulator/BallAndStick/BSAlignedNC.h>
#include <Simulator/BrainRegion/BrainRegion.h>
#include <Simulator/Geometries/Box.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

/**
 * @brief This class defines the characteristics of a brain region, such as
 * geometric shape and physiological content. It works with the components of a
 * ball and stick neural circuit.
 *
 */
class BSAlignedBrainRegion : public BrainRegions::BrainRegion {
  public:
    // size_t ID; // *** Already in BrainRegion
    // std::shared_ptr<Geometries::Box> Shape{};
    // std::shared_ptr<BallAndStick::BSAlignedNC> Content{};

    //! Constructors
    BSAlignedBrainRegion();
    BSAlignedBrainRegion(size_t _ID, Geometries::Box * _Shape, std::shared_ptr<BallAndStick::BSAlignedNC> _Content);

    void Show(float lineWidth);
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
