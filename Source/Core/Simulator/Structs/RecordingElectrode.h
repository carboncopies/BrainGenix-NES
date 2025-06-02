//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the RecordingElectrode struct.
    Additional Notes: None
    Date Created: 2023-10-22
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <cassert>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/Neuron.h>
#include <Simulator/Structs/Simulation.h>

namespace BG {
namespace NES {
namespace Simulator {

// Forward declarations:
struct Simulation;

namespace Tools {

/**
 * @brief This struct emulates a recording electrode.
 * 
 * The RecordingElectrode struct simulates the behavior of a recording electrode
 * used in neuroscience experiments. It calculates electric field potentials
 * and records data from neurons in the simulation.
 */
struct RecordingElectrode {

    std::string Name; /**< Name of the electrode. */
    int ID; /**< Unique identifier for the electrode. */

    Geometries::Vec3D TipPosition_um{0.0, 0.0, 0.0}; /**< Tip position of the electrode in micrometers. */
    Geometries::Vec3D EndPosition_um{0.0, 0.0, 5.0f}; /**< End position of the electrode in micrometers. */
    std::vector<Geometries::Vec3D> Sites{}; /**< List of electrode sites. */
    std::vector<Geometries::Vec3D> SiteLocations_um{}; /**< Locations of sites in micrometers. */

    float NoiseLevel = 1.0; /**< Noise level for the electrode. */
    float SensitivityDampening = 2.0; /**< Sensitivity dampening factor. */

    Simulator::Simulation* Sim; /**< Pointer to the simulation object. */
    std::vector<Geometries::Vec3D> SiteLocations{}; /**< Site locations in the simulation coordinate system. */
    std::vector<std::shared_ptr<CoreStructs::Neuron>> Neurons{}; /**< List of neurons being recorded. */
    std::vector<std::vector<float>> NeuronSomaToSiteDistances_um2{}; /**< Distances from neuron somas to electrode sites in squared micrometers. */
    std::vector<float> TRecorded_ms{}; /**< Time points at which recordings were made in milliseconds. */
    std::vector<std::vector<float>> E_mV{}; /**< Electric field potentials recorded at each site in millivolts. */

    /**
     * @brief Copy constructor for the RecordingElectrode struct.
     * 
     * @param _Electrode Reference to another RecordingElectrode object.
     */
    RecordingElectrode(RecordingElectrode & _Electrode);

    /**
     * @brief Constructor to initialize the RecordingElectrode with a simulation object.
     * 
     * @param _Sim Pointer to the simulation object.
     */
    RecordingElectrode(Simulator::Simulation* _Sim);

    /**
     * @brief Parameterized constructor to initialize the RecordingElectrode.
     * 
     * @param _ID Unique identifier for the electrode.
     * @param _TipPosition_um Tip position of the electrode in micrometers.
     * @param _EndPosition_um End position of the electrode in micrometers.
     * @param _Sites List of electrode sites.
     * @param _NoiseLevel Noise level for the electrode.
     * @param _SensitivityDampening Sensitivity dampening factor.
     * @param _Sim Pointer to the simulation object.
     */
    RecordingElectrode(
        int _ID,
        Geometries::Vec3D _TipPosition_um,
        Geometries::Vec3D _EndPosition_um,
        std::vector<Geometries::Vec3D> _Sites,
        float _NoiseLevel, float _SensitivityDampening,
        Simulator::Simulation* _Sim);

    /**
     * @brief Converts electrode coordinates to the simulation coordinate system.
     * 
     * @param eLocRatio Ratio of the electrode location.
     * @return Geometries::Vec3D Converted coordinates in the simulation system.
     */
    Geometries::Vec3D CoordsElectrodeToSystem(Geometries::Vec3D eLocRatio);

    /**
     * @brief Initializes site locations in the simulation coordinate system.
     */
    void InitSystemCoordSiteLocations();

    /**
     * @brief Initializes references to neurons and calculates distances to electrode sites.
     */
    void InitNeuronReferencesAndDistances();

    /**
     * @brief Initializes recording data structures.
     */
    void InitRecords();

    /**
     * @brief Adds noise to the recorded data.
     * 
     * @return float Noise value.
     */
    float AddNoise();

    /**
     * @brief Calculates the electric field potential at a specific electrode site.
     * 
     * @param siteIdx Index of the electrode site.
     * @return float Electric field potential at the site.
     */
    float ElectricFieldPotential(size_t siteIdx);

    /**
     * @brief Records data at a specific time point.
     * 
     * @param t_ms Time in milliseconds at which to record.
     */
    void Record(float t_ms);

    /**
     * @brief Retrieves the recorded data.
     * 
     * @return std::unordered_map<std::string, std::vector<std::vector<float>>> Map of recorded data.
     */
    std::unordered_map<std::string, std::vector<std::vector<float>>> GetRecording();

    /**
     * @brief Retrieves the recorded data in JSON format.
     * 
     * @return nlohmann::json JSON representation of the recorded data.
     */
    nlohmann::json GetRecordingJSON() const;
};

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
