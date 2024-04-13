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
    Config::Config* Config_; /**Pointer to configuration struct owned by the rest of the system*/

    API::RPCManager* RPCManager_;

    std::vector<std::thread> SimulationThreads_; /**Threads that enumerate simulations and checks for any tasks to be done.*/
    std::atomic<bool> StopThreads_; /**Indicates to workers to stop what they're doing*/
    std::vector<std::unique_ptr<Simulation>> Simulations_; /**Vector containing simulation instances. Index in this vector is the simulation's ID (Also stored in the simulation struct for reference.)*/
    // Note: This simulation vector is not thread safe and will probably segfault if you try to multithread this
    // we will fix this later when we scale the system (DO NOT ALLOW RPC to use more than 1 thread unless this is fixed!)

    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to the instance of the logging system*/
    VSDA::RenderPool* RenderPool_;                        /**Pointer to an instance of the render threadpool class*/
    VisualizerPool* VisualizerPool_;                      /**Pointer to instance of the vizualizerpool class*/

    int NextManTaskID = 0; /**Use this, because we use a map not a vector, so that we can expire some to shed old cached stuff*/
    std::map<int, std::unique_ptr<API::ManagerTaskData>> ManagerTasks; /**Status data of launched tasks by Task ID*/



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
    SimulationRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, Config::Config* _Config, VSDA::RenderPool* _RenderPool, VisualizerPool* _VisualizerPool, API::RPCManager* _RPCManager);

    /**
     * @brief Destroy the SimulationRPCInterface object
     * 
     */
    ~SimulationRPCInterface();

    // Simulation* MakeSimulation();

    // bool BadReqID(int ReqID);

    int AddManagerTask(std::unique_ptr<API::ManagerTaskData> & TaskData);

    void LoadingSimSetter(bool SetTo);
    void SimLoadingTask(API::ManagerTaskData & TaskData);

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
    std::string SimulationGetRecording(std::string _JSONRequest);
    std::string SimulationGetStatus(std::string _JSONRequest);
    std::string SimulationSave(std::string _JSONRequest);
    std::string SimulationGetSave(std::string _JSONRequest);
    std::string SimulationLoad(std::string _JSONRequest);
    std::string SimulationGetGeoCenter(std::string _JSONRequest);

    std::string ManTaskStatus(std::string _JSONRequest);


    std::string AttachRecordingElectrodes(std::string _JSONRequest);
    std::string CalciumImagingAttach(std::string _JSONRequest);
    std::string CalciumImagingShowVoxels(std::string _JSONRequest);
    std::string CalciumImagingRecordAposteriori(std::string _JSONRequest);
    std::string SetRecordInstruments(std::string _JSONRequest);
    std::string GetInstrumentRecordings(std::string _JSONRequest);



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


};

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
