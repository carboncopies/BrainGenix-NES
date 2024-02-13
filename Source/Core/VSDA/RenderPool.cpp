//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/RenderPool.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {



// Thread Main Function
void RenderPool::RendererThreadMainFunction(int _ThreadNumber) {

    Logger_->Log("Started RenderPool Thread " + std::to_string(_ThreadNumber), 0);

    // Run until thread exit is requested - that is, this is set to false
    while (ThreadControlFlag_) {

        // Step 1, Check For Work
        Simulation* SimToProcess = nullptr;
        if (DequeueSimulation(&SimToProcess)) {

            // Okay, we got work, now render this simulation
            Logger_->Log("RenderPool Thread " + std::to_string(_ThreadNumber) + " Detecting Work For Simulation " + std::to_string(SimToProcess->ID), 2);
            if (SimToProcess->VSDAData_.State_ == VSDA_RENDER_REQUESTED) {
                Logger_->Log("RenderPool Thread " + std::to_string(_ThreadNumber) + " Rendering EM Stack For Simulation " + std::to_string(SimToProcess->ID), 2);
                VSDA::ExecuteSubRenderOperations(Logger_, SimToProcess, EMImageProcessorPool_.get(), EMArrayGeneratorPool_.get());
            } else if (SimToProcess->CaData_.State_ == ::BG::NES::VSDA::Calcium::CA_RENDER_REQUESTED) {
                Logger_->Log("RenderPool Thread " + std::to_string(_ThreadNumber) + " Rendering Calcium Stack For Simulation " + std::to_string(SimToProcess->ID), 2);
                ::BG::NES::VSDA::Calcium::ExecuteCaSubRenderOperations(Logger_, SimToProcess, CalciumImageProcessorPool_.get(), CalciumArrayGeneratorPool_.get());
            }
            SimToProcess->IsRendering = false;

        } else {

            // We didn't get any work, just go to sleep for a few milliseconds so we don't rail the cpu
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}


// Constructor, Destructor
RenderPool::RenderPool(BG::Common::Logger::LoggingSystem* _Logger, bool _Windowed, int _NumThreads) {
    assert(_Logger != nullptr);


    // Initialize Variables
    Logger_ = _Logger;
    ThreadControlFlag_ = true;
    Windowed_ = _Windowed;


    // Create VoxelArrayGenerator Instance
    int NumArrayGeneratorThreads = std::thread::hardware_concurrency();
    EMArrayGeneratorPool_ = std::make_unique<VoxelArrayGenerator::ArrayGeneratorPool>(Logger_, NumArrayGeneratorThreads);
    CalciumArrayGeneratorPool_ = std::make_unique<::BG::NES::VSDA::Calcium::VoxelArrayGenerator::ArrayGeneratorPool>(Logger_, NumArrayGeneratorThreads);


    // Create ImageProcessorPool Instance
    int NumEncoderThreads = std::thread::hardware_concurrency();
    EMImageProcessorPool_ = std::make_unique<ImageProcessorPool>(Logger_, NumEncoderThreads);
    CalciumImageProcessorPool_ = std::make_unique<::BG::NES::VSDA::Calcium::ImageProcessorPool>(Logger_, NumEncoderThreads);


    // Create Renderer Instances
    Logger_->Log("Creating RenderPool With " + std::to_string(_NumThreads) + " Thread(s)", 2);
    for (unsigned int i = 0; i < _NumThreads; i++) {
        Logger_->Log("Starting RenderPool Thread " + std::to_string(i), 1);
        RenderThreads_.push_back(std::thread(&RenderPool::RendererThreadMainFunction, this, i));
    }
}

RenderPool::~RenderPool() {

    // Send Stop Signal To Threads
    Logger_->Log("Stopping RenderPool Threads", 2);
    ThreadControlFlag_ = false;

    // Join All Threads
    Logger_->Log("Joining RenderPool Threads", 1);
    for (unsigned int i = 0; i < RenderThreads_.size(); i++) {
        Logger_->Log("Joining RenderPool Thread " + std::to_string(i), 0);
        RenderThreads_[i].join();
    }

}


// Queue Access Functions
void RenderPool::EnqueueSimulation(Simulation* _Sim) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    Queue_.emplace(_Sim);
}

int RenderPool::GetQueueSize() {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    int QueueSize = Queue_.size();

    return QueueSize;
}

bool RenderPool::DequeueSimulation(Simulation** _SimPtr) {

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
void RenderPool::QueueRenderOperation(Simulation* _Sim) {
    EnqueueSimulation(_Sim);
}


}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG