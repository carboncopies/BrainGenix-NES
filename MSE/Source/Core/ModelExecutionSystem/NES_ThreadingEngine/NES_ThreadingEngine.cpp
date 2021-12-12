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


}


// Destructor
NES_CLASS_ThreadingEngine::~NES_CLASS_ThreadingEngine() {


}

