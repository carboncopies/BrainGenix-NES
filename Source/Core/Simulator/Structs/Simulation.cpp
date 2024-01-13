#include <Simulator/Structs/Simulation.h>

#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {

//! Constructors
Simulation::Simulation(BG::Common::Logger::LoggingSystem* _Logger) : Logger_(_Logger) {};

void Simulation::AddCircuit(
    std::shared_ptr<CoreStructs::NeuralCircuit> circuit) {
    assert(circuit != nullptr);
    auto ID = std::to_string(circuit->ID);
    this->NeuralCircuits[ID] = circuit;
};

void Simulation::AddRegion(std::shared_ptr<BrainRegions::BrainRegion> region) {
    auto regionPtr =
        std::dynamic_pointer_cast<BallAndStick::BSAlignedBrainRegion>(region);
    assert(regionPtr != nullptr);
    auto ID = std::to_string(regionPtr->ID);
    this->Regions[ID] = regionPtr;
};

size_t Simulation::GetTotalNumberOfNeurons() {
    size_t long num_neurons = 0;
    for (auto &[circuitID, circuit] : this->NeuralCircuits) {
        num_neurons += circuit->GetNumberOfNeurons();
    }
    return num_neurons;
}

std::vector<std::shared_ptr<CoreStructs::Neuron>> Simulation::GetAllNeurons() {
    std::vector<std::shared_ptr<CoreStructs::Neuron>> allNeurons{};

    for (auto &[circuitID, circuit] : this->NeuralCircuits) {
        auto neurons = circuit->GetNeurons();
        for (auto neuron : neurons)
            allNeurons.emplace_back(neuron);
    }
    return allNeurons;
};

std::vector<size_t> Simulation::GetAllNeuronIDs() {
    std::vector<size_t> allNeuronIDs{};

    for (auto &[circuitID, circuit] : this->NeuralCircuits) {
        auto neurons = circuit->GetNeurons();
        for (auto neuron : neurons)
            allNeuronIDs.emplace_back(neuron->ID);
    }
    return allNeuronIDs;
};

std::vector<std::shared_ptr<CoreStructs::Neuron>>
Simulation::GetNeuronsByIDs(std::vector<size_t> IDList) {
    std::vector<std::shared_ptr<CoreStructs::Neuron>> foundNeurons{};

    for (auto &[circuitID, circuit] : this->NeuralCircuits) {
        auto neurons = circuit->GetNeuronsByIDs(IDList);
        for (auto neuron : neurons)
            foundNeurons.emplace_back(neuron);
    }
    return foundNeurons;
};

Geometries::Vec3D Simulation::GetGeoCenter() {
    Geometries::Vec3D geoCenter_um{};
    size_t numCells = 0;

    for (auto &[circuitID, circuit] : this->NeuralCircuits) {
        auto circuitPtr = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        assert(circuitPtr);

        auto circuitCellCenters = circuitPtr->GetCellCenters();

        for (auto cellCenter : circuitCellCenters)
            geoCenter_um = geoCenter_um + cellCenter;
        numCells += circuitCellCenters.size();
    }
    geoCenter_um = geoCenter_um / static_cast<double>(numCells);
    return geoCenter_um;
};

void Simulation::AttachDirectStim(
    std::vector<std::tuple<float, size_t>> listOfStims) {

    for (auto &[circuitID, circuit] : this->NeuralCircuits) {
        auto circuitPtr =
            std::dynamic_pointer_cast<BallAndStick::BSAlignedNC>(circuit);
        assert(circuitPtr);
        circuitPtr->AttachDirectStim(listOfStims);
    }
}

void Simulation::SetSpontaneousActivity(
    std::vector<std::tuple<float, float, size_t>> spontSpikeSettings) {
    for (auto &[circuitID, circuit] : this->NeuralCircuits) {
        auto circuitPtr = std::dynamic_pointer_cast<
            BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
        assert(circuitPtr);
        circuitPtr->SetSpontaneousActivity(spontSpikeSettings);
    }
};

//! Record all dynamically calculated values for a maximum of tMax_ms
//! milliseconds. Setting t_max_ms=0 effectively turns off recording.
//! Setting t_max_ms to -1 means record forever.
void Simulation::SetRecordAll(float tMax_ms) {
    assert(tMax_ms == _RECORD_FOREVER_TMAX_MS || tMax_ms >= 0.0);

    bool recordingOff = this->InstrumentsMaxRecordTime_ms == 0.0;
    this->InstrumentsMaxRecordTime_ms = tMax_ms;
    if (!recordingOff)
        this->InstrumentsStartRecordTime_ms = this->T_ms;
};

bool Simulation::IsRecording() {

    if (this->InstrumentsMaxRecordTime_ms < 0)
        return true;
    return this->T_ms < (this->InstrumentsStartRecordTime_ms +
                         this->InstrumentsMaxRecordTime_ms);
};

std::unordered_map<std::string, CoreStructs::CircuitRecording>
Simulation::GetRecording() {
    std::unordered_map<std::string, CoreStructs::CircuitRecording> recording;
    for (auto &[circuitID, circuit] : this->NeuralCircuits) {
        auto circuitPtr =
            std::dynamic_pointer_cast<BallAndStick::BSAlignedNC>(circuit);
        assert(circuitPtr);
        recording[circuitID] = circuitPtr->GetRecording();
    }

    return recording;
};

enum sim_methods {
    simmethod_list_of_neurons,
    simmethod_circuits,
    NUMsimmethods,
};

void Simulation::RunFor(float tRun_ms) {
    assert(Logger_ != nullptr);
    
    if (!(tRun_ms >= 0.0))
        return;

    float tEnd_ms = this->T_ms + tRun_ms;

    Logger_->Log("Simulation run includes:", 3);
    Logger_->Log(std::to_string(NeuralCircuits.size())+" circuits with a total of", 3);
    Logger_->Log(std::to_string(Neurons.size())+" neurons drafted in Neurons vector and a total of", 3);
    Logger_->Log(std::to_string(GetTotalNumberOfNeurons())+" neurons residing in defined circuits and a total of", 3);
    Logger_->Log(std::to_string(BSCompartments.size())+" compartments.", 3);

    // *** TODO: obtain this from an API call...
    sim_methods simmethod = simmethod_list_of_neurons;

    while (this->T_ms < tEnd_ms) {
        bool recording = this->IsRecording();
        if (recording)
            this->TRecorded_ms.emplace_back(this->T_ms);
        switch (simmethod) {
            case simmethod_list_of_neurons: {
                for (auto & neuron_ptr : this->Neurons) {
                    if (neuron_ptr) {
                        neuron_ptr->Update(this->T_ms, recording);
                    }
                }
                break;
            }
            case simmethod_circuits: {
                for (auto &[circuitID, circuit] : this->NeuralCircuits) {
                    auto circuitPtr = std::dynamic_pointer_cast<BallAndStick::BSAlignedNC>(circuit);
                    assert(circuitPtr);
                    circuitPtr->Update(this->T_ms, recording);
                }
                break;
            }
        }
        this->T_ms += this->Dt_ms;
    }
};

void Simulation::Show() { return; };

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
