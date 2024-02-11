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

#include <Simulator/EngineController.h>
#include <Config/Config.h>
#include <RPC/Manager.h>
#include <Util/JSONHelpers.h>
#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {

// Forward declarations:
class Manager;

enum ManagerTaskStatus {
    Success = 0, // Completed successfully.
    Active = 1,  // Freshly created and/or actively operating.
    TimeOut = 2, // Failed due to time-out.
    GeneralFailure = 3,
    NUMManagerTaskStatus
};

/**
 * Process:
 * 1. Prepare the data that the Task will need.
 * 2. Launch the specific Task function within a std::thread, giving it a pointer to this struct.
 * 3. Call AddManagerTask with this struct.
 * 4. Use the Task ID to refer to the status and results of the Task, as stored in this struct.
 */
struct ManagerTaskData {
    // Must be set before launching Task:
    Manager & Man;
    std::string InputData;

    // Must be set before calling AddManagerTask:
    std::unique_ptr<std::thread> Task;

    // Is set within AddManagerTask:
    int ID = -1; 

    // Results set by the Task thread:
    ManagerTaskStatus Status = ManagerTaskStatus::Active;
    nlohmann::json OutputData;
    int ReplaceSimulationID = -1;

    ManagerTaskData(Manager & _Man): Man(_Man) {}

    void SetStatus(ManagerTaskStatus _status) { Status = _status; }

    void IncludeStatusInOutputData() { OutputData["TaskStatus"] = int(Status); }

    bool HasReplacementSimID() { return ReplaceSimulationID >= 0; }
};

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
    VisualizerPool* VisualizerPool_;                      /**Pointer to instance of the vizualizerpool class*/

    int NextManTaskID = 0; /**Use this, because we use a map not a vector, so that we can expire some to shed old cached stuff*/
    std::map<int, std::unique_ptr<ManagerTaskData>> ManagerTasks; /**Status data of launched tasks by Task ID*/

    //bool LoadingSim = false;
    //int LoadingSimReplaceID = -1;
    //std::mutex LoadingSimSetterMutex;


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
    Manager(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, VSDA::RenderPool* _RenderPool, VisualizerPool* _VisualizerPool, API::Manager* _RPCManager);

    /**
     * @brief Destroy the Manager object
     * 
     */
    ~Manager();

    Simulation* MakeSimulation();

    bool BadReqID(int ReqID);

    int AddManagerTask(std::unique_ptr<ManagerTaskData> & TaskData);

    //void LoadingSimSetter(bool SetTo);
    void SimLoadingTask(ManagerTaskData & TaskData);

    /**
     * @brief Various routes for API
     * Note that since this file is getting too long, several RPC handlers are being set up - each taking over part of this.
     * See the relevant file in RPCInterface.(cpp/h) in various directories. 
     * 
     * @param _JSONRequest 
     * @return std::string 
     */
    std::string SimulationCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string SimulationReset(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string SimulationRunFor(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string SimulationRecordAll(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string SimulationGetRecording(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string SimulationGetStatus(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string SimulationBuildMesh(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string SimulationSave(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string SimulationLoad(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string SimulationGetGeoCenter(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);

    std::string SphereCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string BulkSphereCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string CylinderCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string BulkCylinderCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string BoxCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string BulkBoxCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);

    std::string BSCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string BulkBSCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);

    std::string StapleCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string ReceptorCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);

    std::string BSNeuronCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);

    std::string PatchClampDACCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string PatchClampDACSetOutputList(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);

    std::string PatchClampADCCreate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string PatchClampADCSetSampleRate(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string PatchClampADCGetRecordedData(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);

    std::string SetSpecificAPTimes(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string SetSpontaneousActivity(std::string _JSONRequest, ManagerTaskData* called_by_manager_task);

    std::string ManTaskStatus(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);
    std::string VisualizerGetImage(std::string _JSONRequest, ManagerTaskData* _CalledByManagerTask = nullptr);
    std::string VisualizerGetImageHandles(std::string _JSONRequest, ManagerTaskData* _CalledByManagerTask = nullptr);
    std::string VisualizerGetStatus(std::string _JSONRequest, ManagerTaskData* _CalledByManagerTask = nullptr);
    std::string VisualizerGenerateImage(std::string _JSONRequest, ManagerTaskData* _CalledByManagerTask = nullptr);

    std::string NESRequest(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr); // Generic JSON-based NES requests.

    std::string Debug(std::string _JSONRequest, ManagerTaskData* called_by_manager_task = nullptr);

    /**
     * @brief Returns true if the simulation is being worked on by another thread.
     * 
     * @param _Sim 
     * @return true 
     * @return false 
     */
    bool IsSimulationBusy(Simulation* _Sim);

    //bool IsLoadingSim() const { return LoadingSim; }

    //bool SimReplaceIDMissing() const { return LoadingSimReplaceID < 0; }

    //int GetSimReplaceID() const { return LoadingSimReplaceID; }

    size_t GetNumberOfSimulations() const { return Simulations_.size(); }

    /**
     * @brief Returns a pointer to the current simulation vector.
     * 
     * @return std::vector<std::unique_ptr<Simulation>>* 
     */
    std::vector<std::unique_ptr<Simulation>>* GetSimulationVectorPtr();

    // This allows use of the Manager Logger.
    BG::Common::Logger::LoggingSystem * Logger() const { return Logger_; }

};

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
