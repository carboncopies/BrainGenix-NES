//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the main simulation manager code.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <memory>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/Sphere.h>
#include <Simulator/Structs/Cylinder.h>
#include <Simulator/Structs/Box.h>
#include <Simulator/Structs/BS.h>
#include <Simulator/Structs/Staple.h>
#include <Simulator/Structs/Receptor.h>
#include <Simulator/Structs/PatchClampDAC.h>
#include <Simulator/Structs/PatchClampADC.h>

#include <Config/Config.h>
#include <RPC/Manager.h>
#include <Util/JSONHelpers.h>


namespace BG {
namespace NES {
namespace Simulator {


/**
 * @brief This class provides the infrastructure to run simulations.
 */
class Manager {

private:
    Config::Config* Config_; /**Pointer to configuration struct owned by rest of system*/

    std::vector<std::thread> SimulationThreads_; /**Threads that enumerate simulations and checks for any tasks to be done.*/
    std::atomic<bool> StopThreads_; /**Indicates to workers to stop what they're doing*/
    std::vector<std::unique_ptr<Simulation>> Simulations_; /**Vector containing simulation instances. Index in this vector is the simulation's ID (Also stored in the simulation struct for reference.)*/
    // Note: This simulation vector is not thread safe and will probably segfault if you try to multithread this
    // we will fix this later when we scale the system (DO NOT ALLOW RPC to use more than 1 thread unless this is fixed!)

public:

    /**
     * @brief Construct a new Manager object
     * Give this a pointer to an initialized configuration object.
     * 
     * @param _Config 
     * @param _RPCManager
     */
    Manager(Config::Config* _Config, API::Manager* _RPCManager);


    /**
     * @brief Destroy the Manager object
     * 
     */
    ~Manager();


    /**
     * @brief Various routes for API
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string SimulationCreate(std::string _JSONRequest);
    std::string SimulationReset(std::string _JSONRequest);
    std::string SimulationRunFor(std::string _JSONRequest);
    std::string SimulationRecordAll(std::string _JSONRequest);
    std::string SimulationGetRecording(std::string _JSONRequest);
    std::string SimulationGetStatus(std::string _JSONRequest);
    std::string SphereCreate(std::string _JSONRequest);
    std::string CylinderCreate(std::string _JSONRequest);
    std::string BoxCreate(std::string _JSONRequest);
    std::string BSCreate(std::string _JSONRequest);
    std::string StapleCreate(std::string _JSONRequest);
    std::string ReceptorCreate(std::string _JSONRequest);
    std::string PatchClampDACCreate(std::string _JSONRequest);
    std::string PatchClampDACSetOutputList(std::string _JSONRequest);
    std::string PatchClampADCCreate(std::string _JSONRequest);
    std::string PatchClampADCSetSampleRate(std::string _JSONRequest);
    std::string PatchClampADCGetRecordedData(std::string _JSONRequest);


    /**
     * @brief Function for thread to run, checks for new updates in each simulation.
     * 
     */
    void SimulationEngineThread(Simulation* _Sim);

    /**
     * @brief Returns true if the simulation is being worked on by another thread.
     * 
     * @param _Sim 
     * @return true 
     * @return false 
     */
    bool IsSimulationBusy(Simulation* _Sim);

};

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG