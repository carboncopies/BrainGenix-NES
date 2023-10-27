#include <cassert>

#include <Simulator/BallAndStick/BSNeuron.h>
#include <Simulator/Structs/RecordingElectrode.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Tools {

//! Constructors
RecordingElectrode::RecordingElectrode(
    std::shared_ptr<Simulator::Simulation> _Sim)
    : Sim(_Sim) {

    this->InitSystemCoordSiteLocations();
    this->InitNeuronReferencesAndDistances();
};

RecordingElectrode::RecordingElectrode(
    int _ID, Geometries::Vec3D _TipPosition_um,
    Geometries::Vec3D _EndPosition_um,
    std::vector<std::tuple<float, float, float>> _Sites, float _NoiseLevel,
    std::shared_ptr<Simulator::Simulation> _Sim)
    : ID(_ID), TipPosition_um(_TipPosition_um), EndPosition_um(_EndPosition_um),
      Sites(_Sites), NoiseLevel(_NoiseLevel), Sim(_Sim) {
    this->InitSystemCoordSiteLocations();
    this->InitNeuronReferencesAndDistances();
};

//! 1. Get a vector from tip to end.
//! 2. Multiply vector coordinates with the ratios given in eloc_ratio.
//! 3. Add the resulting vector to the tip position.
Geometries::Vec3D RecordingElectrode::CoordsElectrodeToSystem(
    std::tuple<float, float, float> eLocRatio) {
    auto toAdd =
        (this->TipPosition_um - this->EndPosition_um) * std::get<2>(eLocRatio);
    return this->TipPosition_um + toAdd;
};

void RecordingElectrode::InitSystemCoordSiteLocations() {
    for (auto &site : this->Sites)
        this->SiteLocations_um.emplace_back(
            this->CoordsElectrodeToSystem(site));
};

void RecordingElectrode::InitNeuronReferencesAndDistances() {
    this->Neurons = this->Sim->GetAllNeurons();
    for (const auto &siteLocation_um : this->SiteLocations_um) {
        std::vector<float> siteDistancesSq_um2{};
        for (const auto neuronPtr : this->Neurons) {
            auto neuron =
                std::dynamic_pointer_cast<BallAndStick::BSNeuron>(neuronPtr);
            assert(neuron);

            auto somaCoord_um = neuron->GetCellCenter();
            float dist = somaCoord_um.Distance(siteLocation_um);
            siteDistancesSq_um2.emplace_back(dist * dist);
        }
        this->NeuronSomaToSiteDistances_um2.emplace_back(siteDistancesSq_um2);
    }
};

void RecordingElectrode::InitRecords() { return; };
float RecordingElectrode::AddNoise() { return 0.0; };

//! Calculate the electric field potential at the electrode site as
//! a combination of the effects of nearby neurons.
float RecordingElectrode::ElectricFieldPotential(int siteIdx) { return 0.0; };
void RecordingElectrode::Record(float t_ms) { return; };
std::unordered_map<std::string, std::vector<float>> GetRecording() {
    return std::unordered_map<std::string, std::vector<float>>();
};

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
