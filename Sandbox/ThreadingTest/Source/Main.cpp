#include <cstdlib>
#include <iostream>
#include <pthread.h>

int NumberThreads = 10;

void *TestFunction(void *ThreadID_) {
    
    // Create ThreadID
    long ThreadID;
    ThreadID = (long)ThreadID_;

    // Print Out Test Message
    std::cout<<"[CHILD] Hello From Thread "<<ThreadID<<std::endl;
    pthread_exit(NULL);

}


int main() {

    // Create Thread Lists
    pthread_t Threads[NumberThreads];
    int ResultCode;
    
    // Create Threads
    for (int i = 0; i < NumberThreads; i++) {
        
        // Log Thread Create
        std::cout<<"[ MAIN] Creating Thread: " << i << std::endl;

        // Create Thread
        ResultCode = pthread_create(&Threads[i], NULL, TestFunction, (void*)i);

        // Check Status Of Thread Creation
        if (ResultCode) {
            std::cout<<"[ MAIN] Failed To Create Thread With ID: " << i << std::endl;
            exit(-1);
        }
    }

    // Without This, Child Threads Would Be Terminated By The OS
    pthread_exit(NULL);

}