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
NES_CLASS_ThreadingEngine::NES_CLASS_ThreadingEngine(int bob) {

    std::cout<<"something boring"<<bob<<std::endl;


    int StartNum = 1;
    int EndNum = 10000;


    int counter = StartNum;
    for (int i = StartNum; i < EndNum; i++) {
        ThreadList_.push_back(std::thread(&NES_CLASS_ThreadingEngine::Test, this, "Something funny :P", i, &counter));
    }

    for (int i = 0; i < ThreadList_.size(); i++) {
        ThreadList_[i].join();
    }

}


// Destructor
NES_CLASS_ThreadingEngine::~NES_CLASS_ThreadingEngine() {


}

void NES_CLASS_ThreadingEngine::Test(const char* msg, int i, int *counter) {

    long double ans = i;
    for (int z = i-1; z > 0; z--) {
        ans*=z;
    }


    while (true) {

        if (*counter == i) {
            std::lock_guard<std::mutex> guard(CanPrint);
            std::cout<<"Input X Value: "<<i<<" = "<<ans<<std::endl;
            *counter = *counter + 1;

            break;
        }
    }


}
