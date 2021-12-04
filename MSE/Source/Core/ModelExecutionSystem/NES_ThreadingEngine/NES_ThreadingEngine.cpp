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


void Test(const char* msg) {

    std::cout<<msg<<std::endl;

}


// Constructor
NES_CLASS_ThreadingEngine::NES_CLASS_ThreadingEngine(int bob) {

    std::cout<<"something boring"<<bob<<std::endl;

    std::thread first (Test, "Something funny :P");
    first.join();

}


// Destructor
NES_CLASS_ThreadingEngine::~NES_CLASS_ThreadingEngine() {


}