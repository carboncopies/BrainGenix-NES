#include <Simulator/Structs/RecordingElectrode.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Tools {

//! Constructors
RecordingElectrode::RecordingElectrode(
    std::shared_ptr<Simulator::Simulation> _Sim)
    : Sim(_Sim) {
    this->Sites.emplace_back(firstSite);
    this->InitSystemCoordSiteLocations();
    this->InitNeuronReferencesAndDistances();
    this->InitRecords();
};

RecordingElectrode::RecordingElectrode(
    int _ID, Geometries::Vec3D _TipPosition_um,
    Geometries::Vec3D _EndPosition_um,
    std::vector<std::tuple<float, float, float>> _Sites, float _NoiseLevel,
    float _SensitivityDampening, std::shared_ptr<Simulator::Simulation> _Sim)
    : ID(_ID), TipPosition_um(_TipPosition_um), EndPosition_um(_EndPosition_um),
      Sites(_Sites), NoiseLevel(_NoiseLevel),
      SensitivityDampening(_SensitivityDampening), Sim(_Sim) {
    this->InitSystemCoordSiteLocations();
    this->InitNeuronReferencesAndDistances();
    this->InitRecords();
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
    for (auto &site : this->Sites) {
        Geometries::Vec3D coords = this->CoordsElectrodeToSystem(site);
        this->SiteLocations_um.emplace_back(coords);
    }
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

void RecordingElectrode::InitRecords() {
    this->E_mV.clear();
    for (size_t i = 0; i < this->Sites.size(); ++i)
        this->E_mV.emplace_back(std::vector<float>());
};

float RecordingElectrode::AddNoise() {
    return this->NoiseLevel * (rand() % 100 / 100.0 - 0.5);
};

//! Calculate the electric field potential at the electrode site as
//! a combination of the effects of nearby neurons.
float RecordingElectrode::ElectricFieldPotential(size_t siteIdx) {
    if (this->SensitivityDampening == 0.0)
        throw std::overflow_error(
            "Cannot divide by zero. (SensitivityDampening)");
    if (siteIdx >= this->NeuronSomaToSiteDistances_um2.size())
        throw std::out_of_range("Out of bounds. (siteIdx)");

    float Ei_mV{};
    auto siteDistances_um = this->NeuronSomaToSiteDistances_um2[siteIdx];

    for (size_t i = 0; i < this->Neurons.size(); ++i) {
        float Vm_mV =
            std::dynamic_pointer_cast<
                BG::NES::Simulator::BallAndStick::BSNeuron>(this->Neurons[i])
                ->Vm_mV;

        if (siteDistances_um[i] <= 1.0)
            Ei_mV = Vm_mV;
        else {
            Ei_mV = Vm_mV / siteDistances_um[i];
            Ei_mV += (Ei_mV / this->SensitivityDampening);
        }
    }

    Ei_mV += this->AddNoise();

    return Ei_mV;
};

void RecordingElectrode::Record(float t_ms) {
    assert(t_ms >= 0.0);
    this->TRecorded_ms.emplace_back(t_ms);
    for (size_t i = 0; i < this->SiteLocations_um.size(); ++i) {
        float Ei_mV = this->ElectricFieldPotential(i);
        (this->E_mV[i]).emplace_back(Ei_mV);
    }
};

std::unordered_map<std::string, std::vector<std::vector<float>>>
RecordingElectrode::GetRecording() {
    std::unordered_map<std::string, std::vector<std::vector<float>>> data{};

    data["E_mV"] = this->E_mV;
    return data;
};

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
