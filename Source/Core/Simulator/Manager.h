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
#include <assert.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Geometries/Cylinder.h>
#include <Simulator/Geometries/Box.h>
#include <Simulator/Structs/BS.h>
#include <Simulator/Structs/Staple.h>
#include <Simulator/Structs/Receptor.h>
#include <Simulator/Structs/Neuron.h>
#include <Simulator/Structs/PatchClampDAC.h>
#include <Simulator/Structs/PatchClampADC.h>

#include <VSDA/DebugHelpers/MeshBuilder.h>
#include <VSDA/RenderPool.h>
#include <BG/Renderer/Interface.h>

#include <Simulator/EngineController.h>
#include <Config/Config.h>
#include <RPC/Manager.h>
#include <Util/JSONHelpers.h>
#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {


/**
 * @brief This class provides the infrastructure to run simulations.
 */
class Manager {

private:
    Config::Config* Config_; /**Pointer to configuration struct owned by the rest of the system*/

    std::vector<std::thread> SimulationThreads_; /**Threads that enumerate simulations and checks for any tasks to be done.*/
    std::atomic<bool> StopThreads_; /**Indicates to workers to stop what they're doing*/
    std::vector<std::unique_ptr<Simulation>> Simulations_; /**Vector containing simulation instances. Index in this vector is the simulation's ID (Also stored in the simulation struct for reference.)*/
    // Note: This simulation vector is not thread safe and will probably segfault if you try to multithread this
    // we will fix this later when we scale the system (DO NOT ALLOW RPC to use more than 1 thread unless this is fixed!)

    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to the instance of the logging system*/
    VSDA::RenderPool* RenderPool_;                        /**Pointer to an instance of the render threadpool class*/

public:

    /**
     * @brief Construct a new Manager object
     * Give this a pointer to an initialized configuration object.
     * 
     * @param _Logger Pointer to the logging interface
     * @param _Config 
     * @param _RPCManager
     * @param _Renderer Instance of the rendering system.
     */
    Manager(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, VSDA::RenderPool* _RenderPool, API::Manager* _RPCManager);

    /**
     * @brief Destroy the Manager object
     * 
     */
    ~Manager();

    bool BadReqID(int ReqID);

    /**
     * @brief Various routes for API
     * Note that since this file is getting too long, several RPC handlers are being set up - each taking over part of this.
     * See the relevant file in RPCInterface.(cpp/h) in various directories. 
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
    std::string SimulationBuildMesh(std::string _JSONRequest);
    std::string SphereCreate(std::string _JSONRequest);
    std::string BulkSphereCreate(std::string _JSONRequest);
    std::string CylinderCreate(std::string _JSONRequest);
    std::string BulkCylinderCreate(std::string _JSONRequest);
    std::string BoxCreate(std::string _JSONRequest);
    std::string BulkBoxCreate(std::string _JSONRequest);
    std::string BSCreate(std::string _JSONRequest);
    std::string BulkBSCreate(std::string _JSONRequest);
    std::string StapleCreate(std::string _JSONRequest);
    std::string ReceptorCreate(std::string _JSONRequest);
    std::string BSNeuronCreate(std::string _JSONRequest);
    std::string PatchClampDACCreate(std::string _JSONRequest);
    std::string PatchClampDACSetOutputList(std::string _JSONRequest);
    std::string PatchClampADCCreate(std::string _JSONRequest);
    std::string PatchClampADCSetSampleRate(std::string _JSONRequest);
    std::string PatchClampADCGetRecordedData(std::string _JSONRequest);

    std::string NESRequest(std::string _JSONRequest); // Generic JSON-based NES requests.

    std::string Debug(std::string _JSONRequest);

    /**
     * @brief Returns true if the simulation is being worked on by another thread.
     * 
     * @param _Sim 
     * @return true 
     * @return false 
     */
    bool IsSimulationBusy(Simulation* _Sim);

    /**
     * @brief Returns a pointer to the current simulation vector.
     * 
     * @return std::vector<std::unique_ptr<Simulation>>* 
     */
    std::vector<std::unique_ptr<Simulation>>* GetSimulationVectorPtr();

};

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
