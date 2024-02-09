//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the simulation struct.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <atomic>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/BallAndStick/BSAlignedBrainRegion.h>
//#include <Simulator/BallAndStick/BSAlignedNC.h>
#include <Simulator/BrainRegion/BrainRegion.h>
#include <Simulator/Geometries/Geometry.h>
#include <Simulator/Geometries/GeometryCollection.h>
#include <Simulator/Geometries/VecTools.h>
#include <Simulator/Structs/BS.h>
#include <Simulator/Structs/NeuralCircuit.h>
#include <Simulator/Structs/Neuron.h>
#include <Simulator/Structs/PatchClampADC.h>
#include <Simulator/Structs/PatchClampDAC.h>
#include <Simulator/Structs/Receptor.h>
#include <Simulator/Structs/Staple.h>
#include <BG/Common/Logger/Logger.h>

#include <Visualizer/VisualizerParameters.h>

#include <VSDA/VoxelSubsystem/Structs/VSDAData.h>


//! The value of tMax_ms at which recording will be done forever.
#define _RECORD_FOREVER_TMAX_MS -1.0

namespace BG {
namespace NES {
namespace Simulator {

enum SimulationActions { SIMULATION_NONE, SIMULATION_RESET, SIMULATION_RUNFOR, SIMULATION_VSDA, SIMULATION_VISUALIZATION};

struct StoredRequest {
    std::string Route;
    std::string RequestJSON;

    StoredRequest(const std::string & _Route, const std::string & _RequestJSON): Route(_Route), RequestJSON(_RequestJSON) {}
    std::string Str() const { return "{ \"" + Route + "\": " + RequestJSON + " }"; }
};

/**
 * @brief Name of the simulation
 *
 */
struct Simulation {
protected:
    std::vector<StoredRequest> StoredRequests;
    int StoredReqID = 0; // Used to create request IDs for stored NESRequests.

public:
    BG::Common::Logger::LoggingSystem* Logger_ = nullptr;

    std::string Name; /**Name of the simulation*/
    int ID;           /**ID of the simulation*/

    float T_ms = 0.0;
    float Dt_ms = 1.0;
    float InstrumentsStartRecordTime_ms = 0.0;
    float InstrumentsMaxRecordTime_ms = 0.0;
    float StartRecordTime_ms = 0.0;
    float MaxRecordTime_ms = 0.0;

    std::unordered_map<std::string, std::shared_ptr<BrainRegions::BrainRegion>> Regions;
    std::unordered_map<std::string, std::shared_ptr<CoreStructs::NeuralCircuit>> NeuralCircuits;

    std::vector<float> TInstruments_ms{};
    std::vector<float> TRecorded_ms{};

    std::string RecordingBlob; /**Blob of json data that contains all recorded states for each thing in the simulation*/

    std::atomic<bool> IsProcessing = false;  /**Indicator if the simulation is currently being modified or not*/
    std::atomic<bool> WorkRequested = false; /**Indicator if work is requested to be done on this simulation by a worker thread*/
    std::atomic<bool> IsRendering = false;   /**Indicates if this simulation is being acted upon by a renderer or not*/
    float RunTimes_ms; /**Number of ms to be simulated next time runfor is called - if not, set to -1*/
    SimulationActions CurrentTask; /**Current task to be processed on this simulation, could be run for, or reset, etc. See above enum for more info.*/

    

    Geometries::GeometryCollection Collection; /**Instance of GeometryCollection struct containing all geometries in this simulation*/

    std::vector<Compartments::BS> BSCompartments; /**This will need to be updated later to a std::variant type, but for now it stores the only type of supported compartments, BallStick type*/
    std::map<int, int> NeuronByCompartment; // Fast look-up map built while creating neurons.

    std::vector<Connections::Staple> Staples; /**List of staple connections, index is their id (also stored in struct)*/
    // The following must contain smart pointers, because content will be
    // moved as the vector is expanded and remapped, and InputReceptorAdded delivers
    // a static pointer:
    std::vector<std::unique_ptr<Connections::Receptor>> Receptors; /**List of receptor connections, index is their id (and it's also stored in the struct itself)*/

    std::vector<std::shared_ptr<CoreStructs::Neuron>> Neurons; /** List of neurons, index is their id. Notice that this takes a Neuron base class object (not BSNeuron and other derivatives). */

    std::vector<Tools::PatchClampDAC> PatchClampDACs; /**List of patchclamp dacs, id is index*/
    std::vector<Tools::PatchClampADC> PatchClampADCs; /**List of patchclamp adcs, id is index*/

    VSDAData VSDAData_; /**Instance of the simulator VSDA data - stores the state for the renderer to use*/

    VisualizerParameters VisualizerParams; /**Instance of visualizer parameters, used to generate visualizations in vulkan*/


    //! Constructors
    Simulation(BG::Common::Logger::LoggingSystem* _Logger);

    void AddCircuit(std::shared_ptr<CoreStructs::NeuralCircuit> circuit);
    void AddRegion(std::shared_ptr<BrainRegions::BrainRegion> region);
    size_t GetTotalNumberOfNeurons();
    std::vector<std::shared_ptr<CoreStructs::Neuron>> GetAllNeurons();
    std::vector<size_t> GetAllNeuronIDs();
    std::vector<std::shared_ptr<CoreStructs::Neuron>>
    GetNeuronsByIDs(std::vector<size_t> IDList);
    Geometries::Vec3D GetGeoCenter();
    void AttachDirectStim(std::vector<std::tuple<float, size_t>> listOfStims);
    void SetSpontaneousActivity(std::vector<std::tuple<float, float, size_t>> spontSpikeSettings);

    unsigned long TotalSpikes() const;

    //! Record all dynamically calculated values for a maximum of tMax_ms
    //! milliseconds. Setting t_max_ms=0 effectively turns off recording.
    //! Setting t_max_ms to -1 means record forever.
    void SetRecordAll(float tMax_ms = _RECORD_FOREVER_TMAX_MS);
    bool IsRecording();
    std::unordered_map<std::string, CoreStructs::CircuitRecording> GetRecording();
    nlohmann::json GetRecordingJSON() const;
    void RunFor(float tRun_ms);
    void Show();

    Compartments::BS * FindCompartmentByID(int CompartmentID);
    CoreStructs::Neuron * FindNeuronByCompartment(int CompartmentID) const;

    std::string WrapAsNESRequest(const std::string & ReqFunc, const std::string & _RequestJSON);
    void StoreRequestHandled(const std::string & ReqFunc, const std::string & _Route, const std::string & _RequestJSON);
    size_t NumStoredRequests() const { return StoredRequests.size(); }
    std::string StoredRequestsToString() const;
    std::string StoredRequestsToNESRequestArray() const;
    std::string StoredRequestsSave() const;
    void ClearStoredRequests() { StoredRequests.clear(); }
};

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
