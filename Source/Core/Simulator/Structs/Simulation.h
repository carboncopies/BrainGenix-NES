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

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/BallAndStick/BSAlignedBrainRegion.h>
#include <Simulator/BallAndStick/BSAlignedNC.h>
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

//! The value of tMax_ms at which recording will be done forever.
#define _RECORD_FOREVER_TMAX_MS -1.0

namespace BG {
namespace NES {
namespace Simulator {

enum SimulationActions { SIMULATION_NONE, SIMULATION_RESET, SIMULATION_RUNFOR };

/**
 * @brief Name of the simulation
 *
 */
struct Simulation {

    std::string Name; /**Name of the simulation*/
    int ID;           /**ID of the simulation*/

    float T_ms = 0.0;
    float Dt_ms = 1.0;
    float InstrumentsStartRecordTime_ms = 0.0;
    float InstrumentsMaxRecordTime_ms = 0.0;
    float StartRecordTime_ms = 0.0;
    float MaxRecordTime_ms = 0.0;

    std::unordered_map<std::string, std::shared_ptr<BrainRegions::BrainRegion>>
        Regions;
    std::unordered_map<std::string, std::shared_ptr<CoreStructs::NeuralCircuit>>
        NeuralCircuits;

    std::vector<float> TInstruments_ms{};
    std::vector<float> TRecorded_ms{};

    std::string RecordingBlob; /**Blob of json data that contains all recorded
                                  states for each thing in the simulation*/

    std::atomic<bool> IsProcessing = false; /**Indicator if the simulation is
                                               currently being modified or not*/
    std::atomic<bool> WorkRequested =
        false; /**Indicator if work is requested to be done on this simulation
                  by a worker thread*/
    float RunTimes_ms; /**Number of ms to be simulated next time runfor is
                          called - if not, set to -1*/
    SimulationActions CurrentTask; /**Current task to be processed on this
                                      simulation, could be run for, or reset,
                                      etc. See above enum for more info.*/

    Geometries::GeometryCollection
        Collection; /**Instance of GeometryCollection struct containing all
                       geometries in this simulation*/

    std::vector<Compartments::BS>
        BSCompartments; /**This will need to be updated later to a std::variant
                           type, but for now it stores the only type of
                           supported compartments, BallStick type*/

    std::vector<Connections::Staple>
        Staples; /**List of staple connections, index is their id (also stored
                    in struct)*/
    std::vector<Connections::Receptor>
        Receptors; /**List of receptor connections, index is their id (and it's
                      also stored in the struct itself)*/

    std::vector<Tools::PatchClampDAC>
        PatchClampDACs; /**List of patchclamp dacs, id is index*/
    std::vector<Tools::PatchClampADC>
        PatchClampADCs; /**List of patchclamp adcs, id is index*/

    //! Constructors
    Simulation();
    Simulation(std::string _Name);

    void AddCircuit(std::shared_ptr<CoreStructs::NeuralCircuit> circuit);
    void AddRegion(std::shared_ptr<BrainRegions::BrainRegion> region);
    std::vector<std::shared_ptr<CoreStructs::Neuron>> GetAllNeurons();
    std::vector<size_t> GetAllNeuronIDs();
    std::vector<std::shared_ptr<CoreStructs::Neuron>>
    GetNeuronsByIDs(std::vector<size_t> IDList);
    Geometries::Vec3D GetGeoCenter();
    void AttachDirectStim(std::vector<std::tuple<float, size_t>> listOfStims);
    void SetSpontaneousActivity(
        std::vector<std::tuple<float, float, size_t>> spontSpikeSettings);

    //! Record all dynamically calculated values for a maximum of tMax_ms
    //! milliseconds. Setting t_max_ms=0 effectively turns off recording.
    //! Setting t_max_ms to -1 means record forever.
    void SetRecordAll(float tMax_ms = _RECORD_FOREVER_TMAX_MS);
    bool IsRecording();
    std::unordered_map<std::string, CoreStructs::CircuitRecording>
    GetRecording();
    void RunFor(float tRun_ms);
    void Show();
};

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
