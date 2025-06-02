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
 * @brief This struct emulates a recording electrode for calcium imaging.
 * 
 * The CalciumImaging struct provides functionality to simulate calcium imaging
 * by recording fluorescence data and initializing relevant parameters.
 */
struct CalciumImaging {

    //! Imaging interval in milliseconds.
    float ImagingInterval_ms = 90.0;

    //! Fluorescence kernel used for imaging.
    std::vector<float> FluorescenceKernel;

    //! Reversed fluorescence kernel used for imaging.
    std::vector<float> ReversedFluorescenceKernel;

    //! Maximum pixel contributions for fluorescence imaging.
    float max_pixel_contributions = 0.0;

    //! Components to include in the imaging process.
    std::vector<int> IncludeComponents;

    //! Recorded time points in milliseconds.
    std::vector<float> TRecorded_ms{};

    /**
     * @brief Initializes the CalciumImaging struct with simulation and microscope parameters.
     * 
     * @param _Sim Pointer to the simulation object.
     * @param _Params Reference to the microscope parameters.
     */
    void Init(Simulation* _Sim, NES::VSDA::Calcium::CaMicroscopeParameters & _Params);

    /**
     * @brief Initializes the fluorescence kernel based on simulation and microscope parameters.
     * 
     * @param _Sim Pointer to the simulation object.
     * @param _Params Reference to the microscope parameters.
     */
    void InitializeFluorescenceKernel(Simulation* _Sim, NES::VSDA::Calcium::CaMicroscopeParameters & _Params);

    /**
     * @brief Initializes the fluorescing neuron FIFOs based on simulation and microscope parameters.
     * 
     * @param _Sim Pointer to the simulation object.
     * @param _Params Reference to the microscope parameters.
     */
    void InitializeFluorescingNeuronFIFOs(Simulation* _Sim, NES::VSDA::Calcium::CaMicroscopeParameters & _Params);

    /**
     * @brief Records fluorescence data at a specific time point.
     * 
     * @param t_ms Time in milliseconds at which to record.
     * @param Sim Pointer to the simulation object.
     * @param _Params Reference to the microscope parameters.
     */
    void Record(float t_ms, Simulation* Sim, NES::VSDA::Calcium::CaMicroscopeParameters& _Params);

};

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
