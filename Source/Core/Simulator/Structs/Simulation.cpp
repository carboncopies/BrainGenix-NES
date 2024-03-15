#include <Simulator/Structs/Simulation.h>

#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/CalciumImaging.h>


#include <iostream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <fstream>

namespace BG {
namespace NES {
namespace Simulator {

//! Constructors
Simulation::Simulation(BG::Common::Logger::LoggingSystem* _Logger) : Logger_(_Logger) {};

/**
 * Takes a fresh random seed and creates a new random generator with that
 * seed. This will be the master random generator for this simulation.
 */
void Simulation::SetRandomSeed(int Seed) {
    RandomSeed = Seed;
    MasterRandom_ = std::make_unique<Distributions::Generic>(Seed);
}

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
    return Neurons.size();
    // size_t long num_neurons = 0;
    // for (auto &[circuitID, circuit] : this->NeuralCircuits) {
    //     num_neurons += circuit->GetNumberOfNeurons();
    // }
    // return num_neurons;
}

std::vector<std::shared_ptr<CoreStructs::Neuron>> Simulation::GetAllNeurons() {
    std::vector<std::shared_ptr<CoreStructs::Neuron>> allNeurons{};
    for (auto & neuron : Neurons) {
        allNeurons.emplace_back(neuron);
    }
    return allNeurons;

    // for (auto &[circuitID, circuit] : this->NeuralCircuits) {
    //     auto neurons = circuit->GetNeurons();
    //     for (auto neuron : neurons)
    //         allNeurons.emplace_back(neuron);
    // }
    // return allNeurons;
};

std::vector<size_t> Simulation::GetAllNeuronIDs() {
    std::vector<size_t> allNeuronIDs{};
    for (auto & neuron : Neurons) {
        allNeuronIDs.emplace_back(neuron->ID);
    }
    return allNeuronIDs;

    // for (auto &[circuitID, circuit] : this->NeuralCircuits) {
    //     auto neurons = circuit->GetNeurons();
    //     for (auto neuron : neurons)
    //         allNeuronIDs.emplace_back(neuron->ID);
    // }
    // return allNeuronIDs;
};

std::vector<std::shared_ptr<CoreStructs::Neuron>>
Simulation::GetNeuronsByIDs(std::vector<size_t> IDList) {
    std::vector<std::shared_ptr<CoreStructs::Neuron>> foundNeurons{};
    for (auto & neuron_id : IDList) {
        if (neuron_id < Neurons.size()) {
            foundNeurons.emplace_back(Neurons.at(neuron_id));
        }
    }
    return foundNeurons;

    // for (auto &[circuitID, circuit] : this->NeuralCircuits) {
    //     auto neurons = circuit->GetNeuronsByIDs(IDList);
    //     for (auto neuron : neurons)
    //         foundNeurons.emplace_back(neuron);
    // }
    // return foundNeurons;
};

Geometries::Vec3D Simulation::GetGeoCenter() const {
    Geometries::Vec3D geoCenter_um{};
    // size_t numCells = 0;

    // for (auto &[circuitID, circuit] : this->NeuralCircuits) {
    //     auto circuitPtr = std::dynamic_pointer_cast<BG::NES::Simulator::BallAndStick::BSAlignedNC>(circuit);
    //     assert(circuitPtr);

    //     auto circuitCellCenters = circuitPtr->GetCellCenters();

    //     for (auto cellCenter : circuitCellCenters) geoCenter_um = geoCenter_um + cellCenter;
    //     numCells += circuitCellCenters.size();
    // }
    // geoCenter_um = geoCenter_um / static_cast<double>(numCells);

    for (const auto & neuron : Neurons) {
        geoCenter_um = geoCenter_um + neuron->GetCellCenter();
    }
    geoCenter_um = geoCenter_um / float(Neurons.size());

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

unsigned long Simulation::TotalSpikes() const {
    unsigned long NumSpikes = 0;
    for (auto & neuron_ptr : this->Neurons) {
        NumSpikes += neuron_ptr->NumSpikes();
    }
    return NumSpikes;
}

//! Record all dynamically calculated values for a maximum of tMax_ms
//! milliseconds. Setting t_max_ms=0 effectively turns off recording.
//! Setting t_max_ms to -1 means record forever.
void Simulation::SetRecordAll(float tMax_ms) {
    assert(tMax_ms == _RECORD_FOREVER_TMAX_MS || tMax_ms >= 0.0);

    bool recordingWasOff = this->MaxRecordTime_ms == 0.0;

    this->MaxRecordTime_ms = tMax_ms;
    if (this->MaxRecordTime_ms != 0) {
        if (!recordingWasOff) this->StartRecordTime_ms = this->T_ms;
    }
};

bool Simulation::IsRecording() const {
    if (this->MaxRecordTime_ms < 0) return true;

    return this->T_ms < (this->StartRecordTime_ms + this->MaxRecordTime_ms);
};

std::unordered_map<std::string, CoreStructs::CircuitRecording> Simulation::GetRecording() {
    std::unordered_map<std::string, CoreStructs::CircuitRecording> recording;
    for (auto &[circuitID, circuit] : this->NeuralCircuits) {
        auto circuitPtr = std::dynamic_pointer_cast<BallAndStick::BSAlignedNC>(circuit);
        assert(circuitPtr);
        recording[circuitID] = circuitPtr->GetRecording();
    }

    return recording;
};

nlohmann::json Simulation::GetRecordingJSON() const {
    nlohmann::json recording;

    recording["t_ms"] = nlohmann::json(this->TRecorded_ms);

    if (!this->NeuralCircuits.empty()) {
        // If there are circuits then obtain the recording from each circuit.
        recording["circuits"] = nlohmann::json::object(); // = {}
        nlohmann::json & circuit_recordings = recording.at("circuits");
        for (const auto &[circuitID, circuit_ptr] : this->NeuralCircuits) {
            assert(circuit_ptr);
            circuit_recordings[circuitID] = circuit_ptr->GetRecordingJSON();
        }

    } else {
        // Otherwise, obtain recordings directly from the list of neurons.
        recording["neurons"] = nlohmann::json::object(); // = {}
        nlohmann::json & neuron_recordings = recording.at("neurons");
        for (const auto & neuron_ptr : this->Neurons) {
            assert(neuron_ptr);
            neuron_recordings[std::to_string(neuron_ptr->ID)] = neuron_ptr->GetRecordingJSON();
        }

    }
    
    return recording;
}

nlohmann::json Simulation::GetCaImagingVoxelsJSON() {
    nlohmann::json voxels;
    return voxels;
}

void Simulation::CalciumImagingRecordAposteriori() {

}

void Simulation::SetRecordInstruments(float tMax_ms) {
    assert(tMax_ms == _RECORD_FOREVER_TMAX_MS || tMax_ms >= 0.0);

    bool recordingWasOff = this->InstrumentsMaxRecordTime_ms == 0.0;

    this->InstrumentsMaxRecordTime_ms = tMax_ms;
    if (this->InstrumentsMaxRecordTime_ms != 0) {
        if (!recordingWasOff) this->InstrumentsStartRecordTime_ms = this->T_ms;
    }
}

bool Simulation::InstrumentsAreRecording() const {
    if (InstrumentsMaxRecordTime_ms < 0 ) return true; // -1 means record forever
    return T_ms < (InstrumentsStartRecordTime_ms + InstrumentsMaxRecordTime_ms);
}

nlohmann::json Simulation::GetInstrumentsRecordingJSON() const {
    nlohmann::json recording;

    recording["t_ms"] = nlohmann::json(this->TInstruments_ms);

    // Virtual experimental functional data from recording electrodes
    if (!RecordingElectrodes.empty()) {
        recording["Electrodes"] = nlohmann::json::object();
        for (const auto & Electrode : RecordingElectrodes) {
            recording["Electrodes"][Electrode->Name] = Electrode->GetRecordingJSON();
        }
    }

    // God's eye functional data about neuron calcium concentrations
    if (CaData_.State_ != BG::NES::VSDA::Calcium::CA_NOT_INITIALIZED) {
        recording["Calcium"] = nlohmann::json::object();
        recording["Calcium"]["Ca_t_ms"] = CaData_.CaImaging.TRecorded_ms;
        if (CaData_.Params_.FlourescingNeuronIDs_.empty()) {
            // All neurons fluoresce.
            for (auto & neuron_ptr : Neurons) {
                BallAndStick::BSNeuron* bsneuron_ptr = static_cast<BallAndStick::BSNeuron*>(neuron_ptr.get());
                recording["Calcium"][std::to_string(bsneuron_ptr->ID)] = bsneuron_ptr->CaSamples;
            }

        } else {
            // For specified fluorescing neurons set.
            for (auto & neuron_id : CaData_.Params_.FlourescingNeuronIDs_) if (neuron_id < Neurons.size()) {
                BallAndStick::BSNeuron* bsneuron_ptr = static_cast<BallAndStick::BSNeuron*>(Neurons.at(neuron_id).get());
                recording["Calcium"][std::to_string(bsneuron_ptr->ID)] = bsneuron_ptr->CaSamples;
            }
        }
    }
    
    return recording;
}

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

    // *** TODO: add making circuits and brain regions
    //           to be able to use the other method

    // *** TODO: obtain this from an API call...
    sim_methods simmethod = simmethod_list_of_neurons;

    unsigned long num_updates_called = 0;
    while (this->T_ms < tEnd_ms) {

        // Track time-points for God's eye recording
        bool recording = this->IsRecording();
        if (recording) {
            //std::cout << 'R'; std::cout.flush();
            this->TRecorded_ms.emplace_back(this->T_ms);
        }

        // Call update in circuits (neurons, etc)
        switch (simmethod) {
            case simmethod_list_of_neurons: {
                //std::cout << "DEBUG --> "; std::cout.flush();
                for (auto & neuron_ptr : this->Neurons) {
                    if (neuron_ptr) {
                        //std::cout << "."; std::cout.flush();
                        neuron_ptr->Update(this->T_ms, recording);
                        num_updates_called++;
                    }
                }
                //std::cout << '\n'; std::cout.flush();
                break;
            }
            case simmethod_circuits: {
                for (auto &[circuitID, circuit] : this->NeuralCircuits) {
                    auto circuitPtr = std::dynamic_pointer_cast<BallAndStick::BSAlignedNC>(circuit);
                    assert(circuitPtr);
                    circuitPtr->Update(this->T_ms, recording);
                    num_updates_called++;
                }
                break;
            }
        }

        // Carry out simulated instrument recordings
        if (InstrumentsAreRecording()) {
            this->TInstruments_ms.emplace_back(this->T_ms);

            // Electrodes
            for (auto & Electrode : RecordingElectrodes) {
                Electrode->Record(this->T_ms);
            }

            // Calcium Imaging
            if (CaData_.State_ != BG::NES::VSDA::Calcium::CA_NOT_INITIALIZED) {
                CaData_.CaImaging.Record(this->T_ms, this, CaData_.Params_); // flatten this later please
            }
        }

        this->T_ms += this->Dt_ms;
    }
    Logger_->Log("Number of top-level Update() calls: "+std::to_string(num_updates_called), 3);
    Logger_->Log("Total number of spikes on all neurons: "+std::to_string(TotalSpikes()), 3);
};

void Simulation::Show() { return; };

Compartments::BS * Simulation::FindCompartmentByID(int CompartmentID) {
    if (CompartmentID >= BSCompartments.size()) {
        return nullptr;
    }

    return &(BSCompartments.at(CompartmentID));
}

CoreStructs::Neuron * Simulation::FindNeuronByCompartment(int CompartmentID) const {
    auto it = NeuronByCompartment.find(CompartmentID);
    if (it == NeuronByCompartment.end()) {
        return nullptr;
    }
    int neuron_id = it->second;
    if ((neuron_id < 0) || (neuron_id >= Neurons.size())) {
        return nullptr;
    }
    return Neurons.at(neuron_id).get();
}

Geometries::Geometry* Simulation::FindShapeByID(int ShapeID) {
    if (ShapeID >= Collection.Size()) {
        return nullptr;
    }

    return Collection.GetGeometry(ShapeID);
}

// std::string Simulation::WrapAsNESRequest(const std::string & ReqFunc, const std::string & _RequestJSON) {
//     std::string wrapped("[ { \"ReqID\": ");
//     wrapped += std::to_string(StoredReqID) + ", \"";
//     wrapped += ReqFunc +"\": ";
//     wrapped += _RequestJSON + " } ]";
//     StoredReqID++;
//     return wrapped;
// }

std::string Simulation::WrapAsNESRequest( const std::string & _RequestJSON, const std::string & _Route) {
    std::string wrapped("{ \"ReqID\": ");
    wrapped += std::to_string(StoredReqID) + ", \"";
    wrapped += _Route + "\": ";
    wrapped += _RequestJSON + " }";
    StoredReqID++;
    return wrapped;
}

// void Simulation::StoreRequestHandled(const std::string & ReqFunc, const std::string & _Route, const std::string & _RequestJSON) {
//     if (_Route.empty()) {
//         // Store it as an NESRequest instead:
//         StoredRequests.emplace_back("NES", WrapAsNESRequest(ReqFunc, _RequestJSON));

//     } else {
//         StoredRequests.emplace_back(_Route, _RequestJSON);
//     }
//     //Logger_->Log("DEBUGGING --> Number of stored requests: "+std::to_string(StoredRequests.size()), 1);
// }

void Simulation::StoreRequestHandled(const std::string & _Route, const std::string & _RequestJSON) {
    // We store everything as a Request within a NESRequest block:
    StoredRequests.emplace_back(_Route, WrapAsNESRequest(_RequestJSON, _Route));
}

std::string Simulation::StoredRequestsToString() const {
    std::stringstream ss;
    for (const auto & storedreq: StoredRequests) {
        ss << storedreq.Str() << '\n';
    }
    return ss.str();
}

std::string Simulation::StoredRequestsToNESRequestArray() const {
    std::stringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < StoredRequests.size(); i++) {
        ss << StoredRequests.at(i).RequestJSON;
        if (i == (StoredRequests.size()-1)) {
            ss << '\n';
        } else {
            ss << ",\n";
        }
    }
    ss << "]\n";
    return ss.str();
}

bool MkDirRecursive(std::string const & dirName, std::error_code & err, bool must_be_new = false) {
    err.clear();
    if (!std::filesystem::create_directories(dirName, err)) {
        if (std::filesystem::exists(dirName)) {
            // The folder already exists:
            if (must_be_new) {
                return false;
            }
            err.clear();
            return true;    
        }
        return false;
    }
    return true;
}

std::string TimeStamp() {
    std::stringstream ss;
    std::time_t t = std::time(nullptr);
    ss << std::put_time(std::localtime(&t), "%F_%T");
    return ss.str();
}

std::string Simulation::StoredRequestsSave() const {
    // Make sure the directory exists.
    std::error_code err;
    if (!MkDirRecursive("SavedSimulations/", err)) {
        return "";
    }

    // Make unique filename.
    std::string FileName = TimeStamp()+'-';
    if (Name.empty()) {
        FileName += "UnNamed";
    } else {
        FileName += Name;
    }

    std::ofstream SaveFile("SavedSimulations/"+FileName+".NES");
    if (!SaveFile.is_open()) {
        return "";
    }
    SaveFile << StoredRequestsToNESRequestArray();
    SaveFile.close();

    return FileName;
}

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
