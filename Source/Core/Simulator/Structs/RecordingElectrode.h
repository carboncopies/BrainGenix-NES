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
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/Neuron.h>
#include <Simulator/Structs/Simulation.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Tools {

/**
 * @brief This struct emulates a recording electrode.
 *
 */
struct RecordingElectrode {

    std::string Name;
    int ID;

    Geometries::Vec3D TipPosition_um{0.0, 0.0, 0.0};
    Geometries::Vec3D EndPosition_um{0.0, 0.0, 5.0f};
    std::vector<Geometries::Vec3D> SiteLocations_um{};
    std::vector<std::tuple<float, float, float>> Sites = {};

    float NoiseLevel = 1.0;
    float SensitivityDampening = 2.0;

    std::shared_ptr<Simulator::Simulation> Sim;
    std::vector<Geometries::Vec3D>
        SiteLocations{}; //! In Simulation coordinate system
    std::vector<std::shared_ptr<CoreStructs::Neuron>> Neurons{};
    std::vector<std::vector<float>>
        NeuronSomaToSiteDistances_um2{}; //!  [ (d_s1n1, d_s1n2, ...), (d_s2n1,
                                         //!  d_s2n2, ...), ...]
    std::vector<float> TRecorded_ms{};   //! [ t0, t1, ... ]
    std::vector<std::vector<float>>
        E_mV{}; //! [ [E1(t0), E1(t1), ...], [E2(t0), E2(t1), ...], ...]

    //! Constructors
    RecordingElectrode(std::shared_ptr<Simulator::Simulation> _Sim);
    RecordingElectrode(int _ID, Geometries::Vec3D _TipPosition_um,
                       Geometries::Vec3D _EndPosition_um,
                       std::vector<std::tuple<float, float, float>> _Sites,
                       float _NoiseLevel, float _SensitivityDampening,
                       std::shared_ptr<Simulator::Simulation> _Sim);

    //! 1. Get a vector from tip to end.
    //! 2. Multiply vector coordinates
    //! 3. Add the resulting vector to the tip position.
    Geometries::Vec3D
    CoordsElectrodeToSystem(std::tuple<float, float, float> eLocRatio);

    void InitSystemCoordSiteLocations();
    void InitNeuronReferencesAndDistances();
    void InitRecords();
    float AddNoise();

    //! Calculate the electric field potential at the electrode site as
    //! a combination of the effects of nearby neurons.
    float ElectricFieldPotential(size_t siteIdx);
    void Record(float t_ms);
    std::unordered_map<std::string, std::vector<float>> GetRecording();
};

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
