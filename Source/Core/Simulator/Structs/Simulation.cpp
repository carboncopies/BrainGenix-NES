#include <Simulator/Structs/Simulation.h>

#include <Simulator/Structs/RecordingElectrode.h>
#include <Simulator/Structs/CalciumImaging.h>
#include <Simulator/SimpleCompartmental/SCNeuron.h>
#include <Simulator/Geometries/GeometryCollection.h>
#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Geometries/Cylinder.h>
#include <Simulator/Geometries/Box.h>



#include <iostream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <memory>

#include <iostream>

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

int Simulation::AddSphere(Geometries::Sphere& _S) {
    _S.ID = Collection.Geometries.size();
    Collection.Geometries.push_back(_S);
    return _S.ID;
}

int Simulation::AddCylinder(Geometries::Cylinder& _S) {
    _S.ID = Collection.Geometries.size();
    Collection.Geometries.push_back(_S);
    return _S.ID;
}

int Simulation::AddBox(Geometries::Box& _S){
    _S.ID = Collection.Geometries.size();
    Collection.Geometries.push_back(_S);
    return _S.ID;
}

/**
 * Note: We cache the pointer to the shape in the compartment data, so that it
 *       does not need to reach back to the Simulation to search for it.
 */
int Simulation::AddSCCompartment(Compartments::BS& _C) {
    _C.ShapePtr = Collection.GetGeometry(_C.ShapeID);
    if (!_C.ShapePtr) {
        return -1;
    }

    _C.ID = BSCompartments.size();
    BSCompartments.push_back(_C);
    return _C.ID;
}

int Simulation::AddSCNeuron(CoreStructs::SCNeuronStruct& _N) {
    if (_N.SomaCompartmentIDs.size()<1) return -1;

    _N.ID = Neurons.size();
    
    Neurons.push_back(std::make_shared<SCNeuron>(_N, *this));
    for (const auto & SomaID : _N.SomaCompartmentIDs) {
        NeuronByCompartment.emplace(SomaID, _N.ID);
    }
    for (const auto & DendriteID : _N.DendriteCompartmentIDs) {
        NeuronByCompartment.emplace(DendriteID, _N.ID);
    }
    for (const auto & AxonID : _N.AxonCompartmentIDs) {
        NeuronByCompartment.emplace(AxonID, _N.ID);
    }

    return _N.ID;
}

struct SaverInfo {
    size_t SGMapSize = 0;
    size_t SphereReferencesSize = 0;
    size_t CylinderReferencesSize = 0;
    size_t BoxReferencesSize = 0;
    size_t BSSCCompartmentsSize = 0;
    size_t NeuronsSize = 0;

    std::string str() const {
        std::string s;
        s += "SGMapSize: " + std::to_string(SGMapSize);
        s += "\nSphereReferencesSize: " + std::to_string(SphereReferencesSize);
        s += "\nCylinderReferencesSize: " + std::to_string(CylinderReferencesSize);
        s += "\nBoxReferencesSize: " + std::to_string(BoxReferencesSize);
        s += "\nBSSCCompartmentsSize: " + std::to_string(BSSCCompartmentsSize);
        s += "\nNeuronsSize: " + std::to_string(NeuronsSize);
        s += '\n';
        return s;
    }
};

struct SaverGeometry {
    Geometries::GeometryShapeEnum Type; // Sphere, Cylinder or Box
    size_t Idx;             // Index within the type-specific list
    SaverGeometry() {}
    SaverGeometry(Geometries::GeometryShapeEnum _Type, size_t _Idx): Type(_Type), Idx(_Idx) {}
};

class Saver {
protected:
    std::string Name_;
    SaverInfo _SaverInfo;
    std::vector<SaverGeometry> SGMap; // Vector indices follow those in Collection.Geometries.
    std::vector<Geometries::Sphere*> SphereReferences;
    std::vector<Geometries::Cylinder*> CylinderReferences;
    std::vector<Geometries::Box*> BoxReferences;
    std::vector<Compartments::BS>* RefToCompartments;
    std::vector<std::shared_ptr<CoreStructs::Neuron>>* RefToSCNeurons;

public:
    Saver(const std::string& _Name): Name_(_Name) {}

    void AddSphere(Geometries::Sphere& S) {
        SGMap.emplace_back(Geometries::GeometrySphere, SphereReferences.size());
        SphereReferences.emplace_back(&S);
    }
    void AddCylinder(Geometries::Cylinder& C) {
        SGMap.emplace_back(Geometries::GeometryCylinder, CylinderReferences.size());
        CylinderReferences.emplace_back(&C);
    }
    void AddBox(Geometries::Box& B) {
        SGMap.emplace_back(Geometries::GeometryBox, BoxReferences.size());
        BoxReferences.emplace_back(&B);
    }
    void AddBSSCCompartments(std::vector<Compartments::BS>& _RefToCompartments) {
        RefToCompartments = &_RefToCompartments;
    }
    //! Warning: This assumes all neurons are of SCNeuron type.
    void AddNeurons(std::vector<std::shared_ptr<CoreStructs::Neuron>>& _RefToSCNeurons) {
        RefToSCNeurons = &_RefToSCNeurons;
    }

    bool Save() {
        /** Save file structure is:
         *  1. SaverInfo
         *  2. SaverGeometry[] (SaverInfo.SGMapSize elements)
         *  3. Geometries::SphereBase[] (SaverInfo.SphereReferencesSize elements)
         *  4. Geometries::CylinderBase[] (SaverInfo.CylinderReferencesSize elements)
         *  5. Geometries::BoxBase[] (SaverInfo.BoxReferencesSize elements)
         *  6. Compartments::BSBaseData[] (SaverInfo.BSSCCompartmentsSize elements)
         *  7. flatdata_sizes[] (SaverInfo.NeuronsSize elements)
         *  8. concatenated SCNeuronStruct::GetFlat()->data() (SaverInfo.NeuronsSize variable size chunks)
         */
        auto SaveFile = std::fstream(Name_, std::ios::out | std::ios::binary);
        _SaverInfo.SGMapSize = SGMap.size();
        _SaverInfo.SphereReferencesSize = SphereReferences.size();
        _SaverInfo.CylinderReferencesSize = CylinderReferences.size();
        _SaverInfo.BoxReferencesSize = BoxReferences.size();
        _SaverInfo.BSSCCompartmentsSize = RefToCompartments->size();
        _SaverInfo.NeuronsSize = RefToSCNeurons->size();

        SaveFile.write((char*)&_SaverInfo, sizeof(_SaverInfo));
        SaveFile.write((char*)SGMap.data(), sizeof(SaverGeometry)*SGMap.size());
        for (auto& ptr : SphereReferences) {
            Geometries::SphereBase& basedataref = *ptr;
            SaveFile.write((char*)&basedataref, sizeof(Geometries::SphereBase));
        }
        for (auto& ptr : CylinderReferences) {
            Geometries::CylinderBase& basedataref = *ptr;
            SaveFile.write((char*)&basedataref, sizeof(Geometries::CylinderBase));
        }
        for (auto& ptr : BoxReferences) {
            Geometries::BoxBase& basedataref = *ptr;
            SaveFile.write((char*)&basedataref, sizeof(Geometries::BoxBase));
        }

        // Save fixed-size base data of compartments.
        for (auto& ref : (*RefToCompartments)) {
            Compartments::BSBaseData& basedataref = ref;
            SaveFile.write((char*)&basedataref, sizeof(Compartments::BSBaseData));
        }

        // Save fixed-size base data of neurons and flattened variable
        // size crucial data.
        std::vector<std::unique_ptr<uint8_t[]>> flatdata_list;
        std::vector<uint32_t> flatdata_sizes; 
        for (auto& ref : (*RefToSCNeurons)) { // from a list of shared pointers to SCNeuron objects
            std::unique_ptr<uint8_t[]> flatdata = static_cast<SCNeuron*>(ref.get())->build_data.GetFlat();
            CoreStructs::SCNeuronStructFlatHeader* header_ptr = (CoreStructs::SCNeuronStructFlatHeader*) flatdata.get();
            flatdata_sizes.push_back(header_ptr->FlatBufSize);
            flatdata_list.emplace_back(flatdata.release());
        }
        SaveFile.write((char*)flatdata_sizes.data(), sizeof(uint32_t)*flatdata_sizes.size());
        for (size_t i = 0; i < flatdata_list.size(); i++) {
            SaveFile.write((char*)flatdata_list.at(i).get(), flatdata_sizes.at(i));
        }

        SaveFile.close();
        return SaveFile.good();
    }
};

class Loader {
public:
    std::string Name_;
    SaverInfo _SaverInfo;
    std::unique_ptr<SaverGeometry[]> SGMap;
    std::unique_ptr<Geometries::SphereBase[]> SphereData;
    std::unique_ptr<Geometries::CylinderBase[]> CylinderData;
    std::unique_ptr<Geometries::BoxBase[]> BoxData;
    std::unique_ptr<Compartments::BSBaseData[]> CompartmentData;
    std::unique_ptr<uint32_t[]> flatdata_sizes;
    std::unique_ptr<uint8_t[]> all_flatdata;

public:
    Loader(const std::string& _Name): Name_(_Name) {}

    bool Load() {
        auto LoadFile = std::fstream(Name_, std::ios::in | std::ios::binary);
        // 1. SaverInfo
        LoadFile.read((char*)&_SaverInfo, sizeof(_SaverInfo));

        // 2. SaverGeometry[] (SaverInfo.SGMapSize elements)
        SGMap = std::make_unique<SaverGeometry[]>(_SaverInfo.SGMapSize);
        LoadFile.read((char*)SGMap.get(), sizeof(SaverGeometry)*_SaverInfo.SGMapSize);
        // 3. Geometries::SphereBase[] (SaverInfo.SphereReferencesSize elements)
        SphereData = std::make_unique<Geometries::SphereBase[]>(_SaverInfo.SphereReferencesSize);
        LoadFile.read((char*)SphereData.get(), sizeof(Geometries::SphereBase)*_SaverInfo.SphereReferencesSize);
        // 4. Geometries::CylinderBase[] (SaverInfo.CylinderReferencesSize elements)
        CylinderData = std::make_unique<Geometries::CylinderBase[]>(_SaverInfo.CylinderReferencesSize);
        LoadFile.read((char*)CylinderData.get(), sizeof(Geometries::CylinderBase)*_SaverInfo.CylinderReferencesSize);
        // 5. Geometries::BoxBase[] (SaverInfo.BoxReferencesSize elements)
        BoxData = std::make_unique<Geometries::BoxBase[]>(_SaverInfo.BoxReferencesSize);
        LoadFile.read((char*)BoxData.get(), sizeof(Geometries::BoxBase)*_SaverInfo.BoxReferencesSize);

        // 6. Compartments::BSBaseData[] (SaverInfo.BSSCCompartmentsSize elements)
        CompartmentData = std::make_unique<Compartments::BSBaseData[]>(_SaverInfo.BSSCCompartmentsSize);
        LoadFile.read((char*)CompartmentData.get(), sizeof(Compartments::BSBaseData)*_SaverInfo.BSSCCompartmentsSize);

        // 7. flatdata_sizes[] (SaverInfo.NeuronsSize elements)
        flatdata_sizes = std::make_unique<uint32_t[]>(_SaverInfo.NeuronsSize);
        LoadFile.read((char*)flatdata_sizes.get(), sizeof(uint32_t)*_SaverInfo.NeuronsSize);
        // 8. concatenated SCNeuronStruct::GetFlat()->data() (SaverInfo.NeuronsSize variable size chunks)
        size_t totsize = 0;
        for (size_t i = 0; i < _SaverInfo.NeuronsSize; i++) totsize += flatdata_sizes.get()[i];
        all_flatdata = std::make_unique<uint8_t[]>(totsize);
        LoadFile.read((char*)all_flatdata.get(), totsize);

        return LoadFile.good();
    }
};

/**
 * Save neuronal circuit specifications to file.
 */
bool Simulation::SaveModel(const std::string& Name) {
    Saver _Saver(Name);
    // Prepare to save shapes.
    for (size_t i = 0; i < Collection.Size(); i++) {
        switch (Collection.GetShapeType(i)) {
        case Geometries::GeometrySphere: {
            auto& S = Collection.GetSphere(i);
            _Saver.AddSphere(S);
            break;
        }
        case Geometries::GeometryCylinder: {
            auto& C = Collection.GetCylinder(i);
            _Saver.AddCylinder(C);
            break;
        }
        case Geometries::GeometryBox: {
            auto& B = Collection.GetBox(i);
            _Saver.AddBox(B);
            break;
        }
        default: {
            Logger_->Log("Encountered a geometric shape for which model saving is not implemented!", 7);
            return false;
        }
        }
    }
    // Prepare to save compartments.
    _Saver.AddBSSCCompartments(BSCompartments);
    // Save neurons.
    _Saver.AddNeurons(Neurons);
    // Save synapses.
    return _Saver.Save();
}

/**
 * Load neuronal circuit specifications from file, replacing any
 * previous specifications in this simulation object.
 */
bool Simulation::LoadModel(const std::string& Name) {
    Loader _Loader(Name);
    if (!_Loader.Load()) return false;

    // Reset and instantiate shapes.
    Collection.Geometries.clear();

    int ID;
    for (size_t i = 0; i < _Loader._SaverInfo.SGMapSize; i++) {
        auto& sgm = _Loader.SGMap.get()[i];
        switch (sgm.Type) {
        case Geometries::GeometrySphere: {
            Geometries::Sphere _S(_Loader.SphereData.get()[sgm.Idx]);
            _S.Name = "sphere-"+std::to_string(i);
            ID = AddSphere(_S);
            break;
        }
        case Geometries::GeometryCylinder: {
            Geometries::Cylinder _S(_Loader.CylinderData.get()[sgm.Idx]);
            _S.Name = "cylinder-"+std::to_string(i);
            ID = AddCylinder(_S);
            break;
        }
        case Geometries::GeometryBox: {
            Geometries::Box _S(_Loader.BoxData.get()[sgm.Idx]);
            _S.Name = "box-"+std::to_string(i);
            ID = AddBox(_S);
            break;
        }
        default: {
            Logger_->Log("Loaded unknown shape type.", 7);
        }
        }
    }

    // Reset and instantiate compartments.
    BSCompartments.clear();

    for (size_t i = 0; i < _Loader._SaverInfo.BSSCCompartmentsSize; i++) {
        Compartments::BS _C(_Loader.CompartmentData.get()[i]);
        _C.Name = "compartment-"+std::to_string(i);
        ID = AddSCCompartment(_C);
    }

    // Reset and instantiate SCNeurons.
    Neurons.clear();

    size_t offset = 0;
    for (size_t i = 0; i < _Loader._SaverInfo.NeuronsSize; i++) {
        CoreStructs::SCNeuronStruct _N;
        _N.FromFlat((CoreStructs::SCNeuronStructFlatHeader*) (_Loader.all_flatdata.get()+offset));
        ID = AddSCNeuron(_N);
        offset += _Loader.flatdata_sizes.get()[i];
    }

    //Show();
    InspectSavedModel(Name);
    return true;
}

/**
 * We can remove this or put it somewhere else as a stand-alone tool.
 * 
 * This uses a different approach to load a saved model file and to
 * inspect its contents. It is meant as a sanity test.
 */
void Simulation::InspectSavedModel(const std::string& Name) const {
    std::filesystem::path savedmodel = Name;
    size_t fsize = std::filesystem::file_size(savedmodel);
    std::cout << "File size: " << fsize << '\n';
    std::vector<uint8_t> data(fsize); 
    auto LoadFile = std::fstream(Name, std::ios::in | std::ios::binary);
    LoadFile.read((char*)data.data(), fsize);
    LoadFile.close();

    SaverInfo* siptr = (SaverInfo*) data.data();
    std::cout << siptr->str();

}

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

nlohmann::json Simulation::GetSpikeTimesJSON() const {
    nlohmann::json spiketimes;

    for (const auto & neuron_ptr : this->Neurons) {
        assert(neuron_ptr);
        spiketimes[std::to_string(neuron_ptr->ID)] = neuron_ptr->GetSpikeTimesJSON();
    }
    
    return spiketimes;
}

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

nlohmann::json Simulation::GetSomaPositionsJSON() const {
    nlohmann::json somapositions;
    somapositions["SomaCenters"] = nlohmann::json::array();
    nlohmann::json& positionslist(somapositions["SomaCenters"]);
    somapositions["SomaTypes"] = nlohmann::json::array();
    nlohmann::json& typeslist(somapositions["SomaTypes"]);

    for (auto& neuron_ptr : Neurons) {
        nlohmann::json vec(nlohmann::json::value_t::array);
        for (auto & element : neuron_ptr->GetCellCenter().AsFloatVector()) {
            vec.push_back(element);
        }
        positionslist.push_back(vec);
        typeslist.push_back(int(neuron_ptr->Type_));
    }

    return somapositions;
}

const std::map<std::string, int> Neurotransmitter2ConnectionType = {
    { "AMPA", 1 },
    { "GABA", 2 },
};

int GetConnectionType(const std::string& neurotransmitter) {
    auto it = Neurotransmitter2ConnectionType.find(neurotransmitter);
    if (it == Neurotransmitter2ConnectionType.end()) return 0; // Unknown type.
    return it->second;
}

nlohmann::json Simulation::GetConnectomeJSON() const {
    nlohmann::json connectome;
    connectome["ConnectionTargets"] = nlohmann::json::array();
    nlohmann::json& targetslist(connectome["ConnectionTargets"]);
    connectome["ConnectionTypes"] = nlohmann::json::array();
    nlohmann::json& typeslist(connectome["ConnectionTypes"]);
    connectome["ConnectionWeights"] = nlohmann::json::array();
    nlohmann::json& weightslist(connectome["ConnectionWeights"]);

    for (auto& neuron_ptr : Neurons) {
        BallAndStick::BSNeuron* bsneuron_ptr = static_cast<BallAndStick::BSNeuron*>(neuron_ptr.get());
        nlohmann::json targetvec(nlohmann::json::value_t::array);
        nlohmann::json typevec(nlohmann::json::value_t::array);
        nlohmann::json weightvec(nlohmann::json::value_t::array);
        for (auto & rdata : bsneuron_ptr->TransmitterDataVec) {
            targetvec.push_back(rdata.DstNeuronID);
            auto ReceptorPtr = rdata.ReceptorPtr;
            typevec.push_back(GetConnectionType(ReceptorPtr->Neurotransmitter));
            weightvec.push_back(ReceptorPtr->Conductance_nS); // *** A better "weight" might by conductance times peak or under-curve area of PSP double-exp.
        }
        targetslist.push_back(targetvec);
        typeslist.push_back(typevec);
        weightslist.push_back(weightvec);
    }

    return connectome;
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

/**
 * Text overview of the state of the current simulation.
 */
void Simulation::Show() {
    std::string simreport("Simulation ID="+std::to_string(ID)+" Name="+Name+" Status Report:\n");
    simreport += "\nNumber of neurons: "+std::to_string(Neurons.size());
    simreport += "\nNumber of compartments: "+std::to_string(BSCompartments.size());
    simreport += "\nNumber of geometric shapes: "+std::to_string(Collection.Geometries.size());
    simreport += "\n\nLocations of neuron somas:\n";
    for (auto& nptr : Neurons) {
        auto& ref = nptr->GetCellCenter();
        //simreport += ref.str() + '\n';
    }
    Logger_->Log(simreport, 3);
    return;
};

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

int Simulation::GetNeuronIndexByCompartment(int CompartmentID) const {
    auto it = NeuronByCompartment.find(CompartmentID);
    if (it == NeuronByCompartment.end()) {
        return -1;
    }
    int neuron_id = it->second;
    if ((neuron_id < 0) || (neuron_id >= Neurons.size())) {
        return -1;
    }
    return neuron_id;
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
