#include <Simulator/Structs/RecordingElectrode.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Tools {

//! Constructors
RecordingElectrode::RecordingElectrode(RecordingElectrode & _Electrode
    ): Name(_Electrode.Name), ID(_Electrode.ID), TipPosition_um(_Electrode.TipPosition_um),
       EndPosition_um(_Electrode.EndPosition_um),
       Sites(_Electrode.Sites), SiteLocations_um(_Electrode.SiteLocations_um), NoiseLevel(_Electrode.NoiseLevel),
       SensitivityDampening(_Electrode.SensitivityDampening), Sim(_Electrode.Sim) {
    assert(Sim != nullptr);
    this->InitSystemCoordSiteLocations();
    this->InitNeuronReferencesAndDistances();
    this->InitRecords();
}

RecordingElectrode::RecordingElectrode(Simulator::Simulation* _Sim): Sim(_Sim) {
    assert(_Sim != nullptr);
    //this->Sites.emplace_back(firstSite);
    this->InitSystemCoordSiteLocations();
    this->InitNeuronReferencesAndDistances();
    this->InitRecords();
};

RecordingElectrode::RecordingElectrode(
    int _ID,
    Geometries::Vec3D _TipPosition_um,
    Geometries::Vec3D _EndPosition_um,
    std::vector<Geometries::Vec3D> _Sites,
    float _NoiseLevel,
    float _SensitivityDampening,
    Simulator::Simulation* _Sim
    ): ID(_ID), TipPosition_um(_TipPosition_um), EndPosition_um(_EndPosition_um),
       Sites(_Sites), NoiseLevel(_NoiseLevel),
       SensitivityDampening(_SensitivityDampening), Sim(_Sim) {
    assert(_Sim != nullptr);
    this->InitSystemCoordSiteLocations();
    this->InitNeuronReferencesAndDistances();
    this->InitRecords();
};

//! 1. Get a vector from tip to end.
//! 2. Multiply vector coordinates with the ratios given in eloc_ratio.
//! 3. Add the resulting vector to the tip position.
// *** TODO: Fix this to properly use the width dimension of the electrode
//       as well and produce a proper conversion.
//       At present, this is using only the z-coord of eloc_ratio as
//       a position along the center line of the vector between tip and end.
Geometries::Vec3D RecordingElectrode::CoordsElectrodeToSystem(Geometries::Vec3D eLocRatio) {
    auto toAdd = (this->TipPosition_um - this->EndPosition_um) * eLocRatio.z;
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
            auto neuron = std::dynamic_pointer_cast<BallAndStick::BSNeuron>(neuronPtr);
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
    return this->NoiseLevel * ((rand() % 100) / 100.0 - 0.5);
};

//! Calculate the electric field potential at the electrode site as
//! a combination of the effects of nearby neurons.
float RecordingElectrode::ElectricFieldPotential(size_t siteIdx) {
    if (this->SensitivityDampening == 0.0)
        throw std::overflow_error(
            "Cannot divide by zero. (SensitivityDampening)");
    if (siteIdx >= this->NeuronSomaToSiteDistances_um2.size())
        throw std::out_of_range("Out of bounds. (siteIdx)");

    float Ei_mV = 0.0;
    auto siteDistances_um = this->NeuronSomaToSiteDistances_um2[siteIdx];

    for (size_t i = 0; i < this->Neurons.size(); ++i) {
        float Vm_mV = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSNeuron>
            (this->Neurons[i])->Vm_mV;

        float Ei_n_mV = (siteDistances_um[i] <= 1.0) ? Vm_mV : Vm_mV / siteDistances_um[i];
        Ei_mV += (Ei_n_mV / this->SensitivityDampening);
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

nlohmann::json RecordingElectrode::GetRecordingJSON() const {
    nlohmann::json data;

    data["E_mV"] = nlohmann::json::array();
    for (unsigned int i = 0; i < this->E_mV.size(); i++) {
        data["E_mV"][i] = this->E_mV[i];
    }

    return data;
}

}; // namespace Tools
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
