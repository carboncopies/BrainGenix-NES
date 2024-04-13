//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Visualizer/Visualizer.h>
#include <Visualizer/VisualizerPool.h>





namespace BG {
namespace NES {
namespace Simulator {



// Thread Main Function
void VisualizerPool::RendererThreadMainFunction(int _ThreadNumber) {

    Logger_->Log("Started VisualizerPool Thread " + std::to_string(_ThreadNumber), 0);

    // Setup Renderer
    BG::NES::Renderer::Interface Renderer(Logger_);
    if (!Renderer.Initialize(Windowed_)) { 
        Logger_->Log("Error During Renderer Initialization, Aborting", 10);
        return;
    }

    // Run until thread exit is requested - that is, this is set to false
    while (ThreadControlFlag_) {

        // Step 1, Check For Work
        Simulation* SimToProcess = nullptr;
        if (DequeueSimulation(&SimToProcess)) {



            // Okay, we got work, now render this simulation
            Logger_->Log("VisualizerPool Thread " + std::to_string(_ThreadNumber) + " Rendering Simulation " + std::to_string(SimToProcess->ID), 2);
            Renderer.ResetScene();
            VisualizeSimulation(Logger_, &Renderer, SimToProcess, ImageProcessorPool_.get());
        


            SimToProcess->IsRendering = false;



        } else {

            // We didn't get any work, just go to sleep for a few milliseconds so we don't rail the cpu
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}


// Constructor, Destructor
VisualizerPool::VisualizerPool(BG::Common::Logger::LoggingSystem* _Logger, bool _Windowed, int _NumThreads) {
    assert(_Logger != nullptr);


    // Initialize Variables
    Logger_ = _Logger;
    ThreadControlFlag_ = true;
    Windowed_ = _Windowed;


    // // Create VoxelArrayGenerator Instance
    // int NumArrayGeneratorThreads = std::thread::hardware_concurrency();
    // ArrayGeneratorPool_ = std::make_unique<VoxelArrayGenerator::ArrayGeneratorPool>(Logger_, NumArrayGeneratorThreads);


    // Create ImageProcessorPool Instance
    int NumEncoderThreads = std::thread::hardware_concurrency();
    ImageProcessorPool_ = std::make_unique<Visualizer::ImageProcessorPool>(Logger_, NumEncoderThreads);


    // Create Renderer Instances
    Logger_->Log("Creating VisualizerPool With " + std::to_string(_NumThreads) + " Thread(s)", 2);
    if (_NumThreads > 1) {
        Logger_->Log("Can't do more than 1 thread due to VSG limitations - will fix this later, sorry.", 10);
    }
    for (unsigned int i = 0; i < _NumThreads; i++) {
        Logger_->Log("Starting VisualizerPool Thread " + std::to_string(i), 1);
        RenderThreads_.push_back(std::thread(&VisualizerPool::RendererThreadMainFunction, this, i));
    }
}

VisualizerPool::~VisualizerPool() {

    // Send Stop Signal To Threads
    Logger_->Log("Stopping VisualizerPool Threads", 2);
    ThreadControlFlag_ = false;

    // Join All Threads
    Logger_->Log("Joining VisualizerPool Threads", 1);
    for (unsigned int i = 0; i < RenderThreads_.size(); i++) {
        Logger_->Log("Joining VisualizerPool Thread " + std::to_string(i), 0);
        RenderThreads_[i].join();
    }

}


// Queue Access Functions
void VisualizerPool::EnqueueSimulation(Simulation* _Sim) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    Queue_.emplace(_Sim);
}

int VisualizerPool::GetQueueSize() {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    int QueueSize = Queue_.size();

    return QueueSize;
}

bool VisualizerPool::DequeueSimulation(Simulation** _SimPtr) {

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


// Public Enqueue Function
void VisualizerPool::QueueRenderOperation(Simulation* _Sim) {
    EnqueueSimulation(_Sim);
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG