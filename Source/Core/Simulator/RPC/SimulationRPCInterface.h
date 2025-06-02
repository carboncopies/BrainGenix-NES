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
#include <mutex>
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

#include <Visualizer/VisualizerPool.h>

#include <RPC/RPCManager.h>
#include <RPC/ManagerTaskData.h>
#include <RPC/RPCHandlerHelper.h>
#include <RPC/RouteAndHandler.h>

#include <Simulator/EngineController.h>
#include <Config/Config.h>
#include <Util/JSONHelpers.h>
#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {

/**
 * @brief This class provides the infrastructure to run simulations.
 */
class SimulationRPCInterface {

private:
    Config::Config* Config_; /**< Pointer to configuration struct owned by the rest of the system */

    API::RPCManager* RPCManager_; /**< Pointer to the RPC manager */

    std::vector<std::thread> SimulationThreads_; /**< Threads that enumerate simulations and check for any tasks to be done */
    std::atomic<bool> StopThreads_; /**< Indicates to workers to stop what they're doing */
    std::vector<std::unique_ptr<Simulation>> Simulations_; /**< Vector containing simulation instances. Index in this vector is the simulation's ID (also stored in the simulation struct for reference) */
    /**< Note: This simulation vector is not thread-safe and may cause segmentation faults if accessed by multiple threads. */

    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**< Pointer to the instance of the logging system */
    VSDA::RenderPool* RenderPool_; /**< Pointer to an instance of the render thread pool class */
    VisualizerPool* VisualizerPool_; /**< Pointer to an instance of the visualizer pool class */

    int NextManTaskID = 0; /**< Task ID counter for manager tasks */
    std::map<int, std::unique_ptr<API::ManagerTaskData>> ManagerTasks; /**< Status data of launched tasks by Task ID */

public:

    /**
     * @brief Construct a new SimulationRPCInterface object.
     * 
     * @param _Logger Pointer to the logging interface
     * @param _Config Pointer to the configuration object
     * @param _RenderPool Pointer to the render pool instance
     * @param _VisualizerPool Pointer to the visualizer pool instance
     * @param _RPCManager Pointer to the RPC manager
     */
    SimulationRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, VSDA::RenderPool* _RenderPool, VisualizerPool* _VisualizerPool, API::RPCManager* _RPCManager);

    /**
     * @brief Destroy the SimulationRPCInterface object.
     */
    ~SimulationRPCInterface();

    /**
     * @brief Add a manager task.
     * 
     * @param TaskData Unique pointer to the task data
     * @return int Task ID
     */
    int AddManagerTask(std::unique_ptr<API::ManagerTaskData>& TaskData);

    /**
     * @brief Set the loading simulation flag.
     * 
     * @param SetTo Boolean value to set the flag
     */
    void LoadingSimSetter(bool SetTo);

    /**
     * @brief Perform a simulation loading task.
     * 
     * @param TaskData Reference to the task data
     */
    void SimLoadingTask(API::ManagerTaskData& TaskData);

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
    std::string SimulationSetSeed(std::string _JSONRequest);
    std::string SimulationRunFor(std::string _JSONRequest);
    std::string SimulationRecordAll(std::string _JSONRequest);
    std::string SimulationGetSpikeTimes(std::string _JSONRequest);
    std::string SimulationGetRecording(std::string _JSONRequest);
    std::string SimulationGetStatus(std::string _JSONRequest);
    std::string SimulationSave(std::string _JSONRequest);
    std::string SimulationGetSave(std::string _JSONRequest);
    std::string SimulationLoad(std::string _JSONRequest);
    std::string SimulationSaveModel(std::string _JSONRequest);
    std::string SimulationLoadModel(std::string _JSONRequest);
    std::string SimulationGetGeoCenter(std::string _JSONRequest);
    std::string SimulationGetBoundingBox(std::string _JSONRequest);

    std::string ManTaskStatus(std::string _JSONRequest);

    std::string SetPrePostStrength(std::string _JSONRequest);
    std::string SetAllStrength(std::string _JSONRequest);
    std::string BatchSetPrePostStrength(std::string _JSONRequest);

    std::string AttachRecordingElectrodes(std::string _JSONRequest);
    std::string CalciumImagingAttach(std::string _JSONRequest);
    std::string CalciumImagingShowVoxels(std::string _JSONRequest);
    std::string CalciumImagingRecordAposteriori(std::string _JSONRequest);
    std::string SetRecordInstruments(std::string _JSONRequest);
    std::string GetInstrumentRecordings(std::string _JSONRequest);

    std::string GetSomaPositions(std::string _JSONRequest);
    std::string GetConnectome(std::string _JSONRequest);
    std::string GetAbstractConnectome(std::string _JSONRequest);


    /**
     * @brief Check if a simulation is busy.
     * 
     * @param _Sim Pointer to the simulation
     * @return true If the simulation is busy
     * @return false If the simulation is not busy
     */
    bool IsSimulationBusy(Simulation* _Sim);

    /**
     * @brief Get the number of simulations.
     * 
     * @return size_t Number of simulations
     */
    size_t GetNumberOfSimulations() const { return Simulations_.size(); }

    /**
     * @brief Get a pointer to the simulation vector.
     * 
     * @return std::vector<std::unique_ptr<Simulation>>* Pointer to the simulation vector
     */
    std::vector<std::unique_ptr<Simulation>>* GetSimulationVectorPtr();


};

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
