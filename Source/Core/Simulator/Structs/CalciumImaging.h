//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the CalciumImaging struct.
    Additional Notes: None
    Date Created: 2024-02-11
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <cassert>
#include <memory>
#include <vector>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/VecTools.h>

namespace BG {
namespace NES {
namespace Simulator {

// Forward declarations:
struct Simulation;

namespace Tools {

/**
 * @brief This struct emulates a recording electrode.
 *
 */
struct CalciumImaging {

    std::string Name;
    int ID;

    std::vector<int> FluorescingNeurons;
    std::string CalciumIndicator;
    float IndicatorRise_ms = 30.0;
    float IndicatorDecay_ms = 80.0;
    float IndicatorInterval_ms = 80.0;
    int VoxelSpaceSide_px = 2;
    bool GenerateDuringSim = false;
    Geometries::Vec3D Center_um{0.0, 0.0, 0.0};
    Geometries::Vec3D Half_um{10.0, 10.0, 2.0};
    Geometries::Vec3D Dx{1.0, 0.0, 0.0};
    Geometries::Vec3D Dy{0.0, 1.0, 0.0};
    Geometries::Vec3D Dz{0.0, 0.0, 1.0}; // Positive dz indicates most visible top surface.

};

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
