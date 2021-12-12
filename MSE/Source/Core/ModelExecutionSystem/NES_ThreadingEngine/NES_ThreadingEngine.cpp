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


// void NES_CLASS_ThreadingEngine::Test(const char* msg, int i, int *counter) {

//     long double ans = i;
//     for (int z = i-1; z > 0; z--) {
//         ans*=z;
//     }


//     while (true) {

//         if (*counter == i) {
//             std::lock_guard<std::mutex> guard(CanPrint);
//             std::cout<<"Input X Value: "<<i<<" = "<<ans<<std::endl;
//             *counter = *counter + 1;

//             break;
//         }
//     }


// }
