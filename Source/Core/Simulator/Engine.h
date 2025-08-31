//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the simulation engine code.
    Additional Notes: None
    Date Created: 2023-06-29
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <memory>
#include <assert.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <queue>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>

#include <Simulator/Updaters/PatchClampADC.h>
#include <Simulator/Updaters/PatchClampDAC.h>
#include <Simulator/Updaters/Receptor.h>
#include <Simulator/Updaters/Staple.h>


namespace BG {
namespace NES {
namespace Simulator {



/**
 * @brief This class provides the infrastructure to run simulations.
 */
class Engine {
public:
    struct SimulationSubset {
        Simulation* Sim_;
        int Start_;
        int End_;
    };

private:

    BG::Common::Logger::LoggingSystem*                        Logger_ = nullptr;   /**Pointer to instance of logging system*/

    std::mutex                                                QueueMutex_;         /**Mutex used to lock access to the queue when it's being modified*/
    std::queue<SimulationSubset>                                   Queue_;              /**Queue that contains simulations that need to be rendered*/

    std::vector<std::thread>                                  SimThreads_;      /**List of simulation threads - each one tries to dequeue stuff from the queue to work on.*/
    std::atomic_bool                                          ThreadControlFlag_;  /**Bool that signals threads to exit*/

    /**
     * @brief Thread safe get simulation* from queue function. 
     * Will return false if there is nothing to be dequeued.
     * Otherwise will update the ptr given as a parameter.
     * 
     * @return true
     * @return false
     */
    bool DequeueSimulation(SimulationSubset* _SimPtr);

    /**
     * @brief Entry point for simulation threads.
     * 
     * @brief _ThreadNumber ID of this thread
     */
    void DynamicSimulationThreadMainFunction(int _ThreadNumber);

    /**
     * @brief Thread safe getSize function.
     * 
     * @return int 
     */
    int GetQueueSize();

public:

    Engine(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads);

    ~Engine();


    /**
     * @brief Resets all attributes of the given simulation which need to be cleared, (this includes any recorded data).
     * 
     */
    void Reset(Simulation* _Sim);

    /**
     * @brief Runs the simulation for the amount of time specified in the simulation's struct.
     * 
     */
    void RunFor(Simulation* _Sim);

    /**
     * @brief Thread safe enqueue function.
     * 
     * @param _Sim 
     */
    void EnqueueSimulation(Simulation* _Sim);

    /**
     * @brief Waits until the queue is empty.
     * NOTE: this does not guarentee everything is done, that would involve us having to ensure all threads completed the work too.
     * We're just waiting until the queue is empty, those threads could still be doing stuff. 
     * 
     */
    void BlockUntilQueueEmpty(bool _LogOutput = true);
};

}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG