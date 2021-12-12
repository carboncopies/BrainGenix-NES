//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file is responsible for implementing the model multithreading feature.
    Documentation Status: Complete
    Additonal Notes: None
    Date Created: 2021-12-03
*/ 

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <NES_LoggingSystem.h>
#include <NES_EmptyNeuronModel.h>


class NES_CLASS_ThreadingEngine {

    // Private Vars
    private:

        // Neuron Vector(s) will need to add more vectors here for other neuron types in the future
        std::vector<std::vector<EmptyNeuronModel>> NeuronModelVector_EmptyNeuronModel; /**<Vector Contaiining Empty Neuron Models*/
        std::mutex ThreadBlockMutexes_[65535]; /*<<Thread Block Mutex Array*/
        std::mutex ThreadExitMutexes_[65535]; /*<<Vector To Exit Threads*/


        // Member Vars
        NES_CLASS_LoggerClass *Logger_; /**<Logger Instance Pointer*/
        std::vector<std::thread> ThreadList_; /**<Vector containing worker threads.*/

        int CPUCount_; /**<Number of cpu cores in system*/



    // Private Methods
    private:
        void WorkerThread(std::vector<EmptyNeuronModel>* Neurons, std::mutex* BlockThread, std::mutex* ExitThread);


    // Public Functions
    public:

        /**
         * @brief Construct a new nes class threadingengine object
         * 
         */
        NES_CLASS_ThreadingEngine(NES_CLASS_LoggerClass *Logger);


        /**
         * @brief Destroy the nes class threadingengine object
         * 
         */
        ~NES_CLASS_ThreadingEngine();

        /**
         * @brief Create the given number of neurons and add them to the neurons vector. NOTE: THE NUMBER OF NEURONS MUST BE A MULTIPLE OF THE NUMBER OF THREADS!
         * @note This is temporary, and will need to be updated later to add multiscale support!
         */
        void CreateNeurons(int NumberNeurons);



        //void Test(const char* Msg, int i, int *counter);

};