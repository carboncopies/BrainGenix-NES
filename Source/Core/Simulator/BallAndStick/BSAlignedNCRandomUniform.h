//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
 * Description: This file provides the BSAlignedNCRandomUniform struct,
 * representing a ball-and-stick neural circuit variant with neuron coordinates
 * drawn from a uniform distribution.
 *
 * Additional Notes: None
 *
 * Date Created: 2023-10-30
 */

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <Simulator/BallAndStick/BSAlignedNC.h>
#include <Simulator/Geometries/VecTools.h>

//! Default soma radius for initializing somas
#define _DEFAULT_SOMA_RADIUS_um 0.5
//! Default end 0 radius for initializing axons
#define _DEFAULT_END0_RADIUS_um 0.1
//! Default end 1 radius for initializing axons
#define _DEFAULT_END1_RADIUS_um 0.1

namespace BG {
namespace NES {
namespace Simulator {
namespace BallAndStick {

/**
 * @brief This struct provides the structure representing a ball-and-stick
 * neural circuit, but with multiple neurons connected and distributed such
 * that their coordinates are drawn from a uniform distribution.
 *
 */
struct BSAlignedNCRandomUniform : BSAlignedNC {

    size_t Seed = 42; //! Seed of the random number generator.

    //! Constructors
    BSAlignedNCRandomUniform(int _ID);
    BSAlignedNCRandomUniform(int _ID, int _NumCells);
    BSAlignedNCRandomUniform(int _ID, int _NumCells, size_t _Seed);

    //! Initializes the neurons in the neural circuit.
    void InitCells(std::shared_ptr<Geometries::Geometry> domain) override;
};

}; // namespace BallAndStick
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
