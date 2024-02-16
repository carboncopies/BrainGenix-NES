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

#include <VSDA/Ca/VoxelSubsystem/Structs/CaMicroscopeParameters.h>

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

    // Simulator::Simulation* Sim;

    // std::string Name;
    // int ID;

    //std::vector<int> FluorescingNeurons;
    //std::string CalciumIndicator;
    //float IndicatorRise_ms = 30.0;
    //float IndicatorDecay_ms = 80.0;
    //float IndicatorInterval_ms = 80.0;
    float ImagingInterval_ms = 90.0;
    //int VoxelSpaceSide_px = 2;
    //float Voxel_um = 0.0;
    //bool GenerateDuringSim = false;
    //Geometries::Vec3D Center_um{0.0, 0.0, 0.0};
    //Geometries::Vec3D Half{10.0, 10.0, 2.0};
    //Geometries::Vec3D Dx{1.0, 0.0, 0.0};
    //Geometries::Vec3D Dy{0.0, 1.0, 0.0};
    //Geometries::Vec3D Dz{0.0, 0.0, 1.0}; // Positive dz indicates most visible top surface.

    std::vector<float> FluorescenceKernel;
    std::vector<float> ReversedFluorescenceKernel;
    float max_pixel_contributions = 0.0;
    // std::vector<float> image_dims_px; // *** or unsigned int?
    //??? image_t; // Image taken at time t.
    //std::vector<???> images;

    std::vector<int> IncludeComponents;

    // *** (in prototype:) voxelspace = []

    std::vector<float> TRecorded_ms{};
    // size_t num_samples = 0;

    //! Constructors
    //CalciumImaging(CalciumImaging & CaImg);
    //CalciumImaging(Simulator::Simulation* _Sim);

    void Init(Simulation* _Sim, NES::VSDA::Calcium::CaMicroscopeParameters & _Params);

    // void SetImageSize();
    // void InstantiateVoxelSpace();
    // void InitializeDepthDimming();
    // void InitializeProjectionCircles();

    void InitializeFluorescenceKernel(Simulation* _Sim, NES::VSDA::Calcium::CaMicroscopeParameters & _Params);
    void InitializeFluorescingNeuronFIFOs(Simulation* _Sim, NES::VSDA::Calcium::CaMicroscopeParameters & _Params);

    void Record(float t_ms, Simulation* Sim, NES::VSDA::Calcium::CaMicroscopeParameters& _Params);

};

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
