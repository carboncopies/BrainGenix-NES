#include <Simulator/Engine.h>



namespace BG {
namespace NES {
namespace Simulator {

void Engine::DynamicSimulationThreadMainFunction(int _ThreadNumber) {
    Logger_->Log("Started SimulationPool Thread " + std::to_string(_ThreadNumber), 0);

    // Run until thread exit is requested - that is, this is set to false
    while (ThreadControlFlag_) {
        // Step 1, Check For Work
        SimulationSubset SimToProcess;
        if (DequeueSimulation(&SimToProcess)) {
            auto Sim = SimToProcess.Sim_;
            // Okay, we got work, now render this simulation
            Logger_->Log("SimulationPool Thread " + std::to_string(_ThreadNumber) + " Starting Update For { " + std::to_string(SimToProcess.Start_) + ", "+ std::to_string(SimToProcess.End_) + " } - ID: " + std::to_string(Sim->ID), 5);

            bool recording = Sim->IsRecording();
            for (int i = SimToProcess.Start_; i <= SimToProcess.End_; i++) {
                auto neuron_ptr = Sim->Neurons[i];
                if (neuron_ptr) {
                    neuron_ptr->Update(Sim->T_ms, recording);
                }
            }
        } else {

            // We didn't get any work, just go to sleep for a few milliseconds so we don't rail the cpu
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

Engine::Engine(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads) {
    assert(_Logger != nullptr);

    // Initialize Variables
    Logger_ = _Logger;

    if (_NumThreads == 0) {
        return;
    }

    ThreadControlFlag_ = true;

    // Create Renderer Instances
    Logger_->Log("Creating SimulationPool With " + std::to_string(_NumThreads) + " Thread(s)", 2);
    for (unsigned int i = 0; i < _NumThreads; i++) {
        Logger_->Log("Starting SimulationPool Thread " + std::to_string(i), 1);
        SimThreads_.push_back(std::thread(&Engine::DynamicSimulationThreadMainFunction, this, i));
    }
}

Engine::~Engine() {
    if (SimThreads_.empty()) {
        return;
    }

    // Send Stop Signal To Threads
    Logger_->Log("Stopping SimulationPool Threads", 2);
    ThreadControlFlag_ = false;

    // Join All Threads
    Logger_->Log("Joining SimulationPool Threads", 1);
    for (unsigned int i = 0; i < SimThreads_.size(); i++) {
        Logger_->Log("Joining SimulationPool Thread " + std::to_string(i), 0);
        SimThreads_[i].join();
    }
}

// Queue Access Functions
void Engine::EnqueueSimulation(Simulation* _Sim) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    int NumSims = _Sim->Neurons.size();
    int NumSubThreads = 5;

    for (int i = 0; i < NumSims; i += NumSubThreads) {
        Queue_.emplace(_Sim, i, std::min(i + NumSubThreads - 1, NumSims - 1));
    }
}

int Engine::GetQueueSize() {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    int QueueSize = Queue_.size();

    return QueueSize;
}

bool Engine::DequeueSimulation(SimulationSubset* _SimPtr) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    // If the queue isn't empty, we grab the first element
    if (Queue_.size() > 0) {
        *_SimPtr = Queue_.front();
        Queue_.pop();

        return true;
    }

    return false;
}


void Engine::BlockUntilQueueEmpty(bool _LogOutput) {

    // Get some var to keep track of the queue length, we're going to be updating this later, but it's fine for now
    int QueueLength = 1;

    while (QueueLength > 0) {

        // We're doing another set of curly brackets to make the mutex go out of scope otherwise it will block forever
        {
            // Firstly, Ensure Nobody Else Is Using The Queue, then update the queue size
            std::lock_guard<std::mutex> LockQueue(QueueMutex_);

            QueueLength = Queue_.size();
        }

        // Wait for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        // Log Queue Size
        Logger_->Log("SimulationPool Queue Length '" + std::to_string(QueueLength) + "'", 1, _LogOutput);

    }

}

void Engine::Reset(Simulation* _Sim) {
    assert(_Sim != nullptr && "Engine::Reset was passed a null _Sim (Simulation*) pointer!");

    // Reset Tools
    for (unsigned int i = 0; i < _Sim->PatchClampDACs.size(); i++) {
        Updater::PatchClampDACReset(&_Sim->PatchClampDACs[i], _Sim);
    }
    for (unsigned int i = 0; i < _Sim->PatchClampADCs.size(); i++) {
        Updater::PatchClampADCReset(&_Sim->PatchClampADCs[i], _Sim);
    }

    // Reset Connections
    for (unsigned int i = 0; i < _Sim->Receptors.size(); i++) {
        Updater::ReceptorReset(_Sim->Receptors.at(i).get(), _Sim);
    }
    for (unsigned int i = 0; i < _Sim->Staples.size(); i++) {
        Updater::StapleReset(&_Sim->Staples[i], _Sim);
    }

}

void Engine::RunFor(Simulation* _Sim) {
    assert(_Sim != nullptr && "Engine::RunFor was passed a null _Sim (Simulation*) pointer!");

    if (!_Sim->MultithreadedSimulation) {
        _Sim->RunFor(_Sim->RunTimes_ms);
    } else {
        _Sim->RunForMT(_Sim->RunTimes_ms, this);
    }
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG