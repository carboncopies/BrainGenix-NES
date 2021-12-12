//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file is responsible for implementing the model multithreading feature.
    Documentation Status: Complete
    Additonal Notes: None
    Date Created: 2021-12-03
*/ 

#include <NES_ThreadingEngine.h>



// Constructor
NES_CLASS_ThreadingEngine::NES_CLASS_ThreadingEngine(NES_CLASS_LoggerClass *Logger) {

    // Copy Logger Pointer
    Logger_ = Logger;

    // Log Init
    Logger_->Log("Initializing Threading Engine", 5);

    // Detect CPU Cores
    Logger_->Log("Detecing Number Of CPU Cores", 4);
    CPUCount_ = std::thread::hardware_concurrency();
    Logger_->Log(std::string(std::string("Found ") + std::to_string(CPUCount_) + std::string(" CPU Cores")).c_str(), 4);


    // Setup Empty Neuron Model Vectors
    Logger_->Log("Popuplating Neuron Assignments To Worker Threads", 4);
    for (int i = 0; i < CPUCount_; i++) {

        // Create Empty Vector
        std::vector<EmptyNeuronModel> EmptyVec;
        NeuronModelVector_EmptyNeuronModel.push_back(EmptyVec);

    }

}

// Destructor
NES_CLASS_ThreadingEngine::~NES_CLASS_ThreadingEngine() {


}

// Create Neurons
void NES_CLASS_ThreadingEngine::CreateNeurons(int NumberNeurons) {

    // Log Creation
    std::string LogString = std::string("Creating ") + std::to_string(NumberNeurons) + std::string(" Neurons");
    Logger_->Log(LogString.c_str(), 5);

    // Calculate Number Of Neurons Per Worker
    int NumberNeuronsPerThread = NumberNeurons / CPUCount_;

    // Loop Through Worker Threads' Neuron Vectors
    for (int ThreadIndex = 0; ThreadIndex < CPUCount_; ThreadIndex++) {

        // Create Neuron
        for (int NeuronID = 0; NeuronID < NumberNeuronsPerThread; NeuronID++) {

            // Setup Neuron
            EmptyNeuronModel Neuron = EmptyNeuronModel();

            // Add To Vector
            NeuronModelVector_EmptyNeuronModel[ThreadIndex].push_back(Neuron);

        }

    }

}