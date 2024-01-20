//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>
#include <VSDA/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VoxelArrayGenerator {



// Thread Main Function
void ArrayGeneratorPool::RendererThreadMainFunction(int _ThreadNumber) {

    Logger_->Log("Started ArrayGeneratorPool Thread " + std::to_string(_ThreadNumber), 0);

    // Run until thread exit is requested - that is, this is set to false
    while (ThreadControlFlag_) {

        // Step 1, Check For Work
        Task* ThisTask = nullptr;
        if (DequeueTask(&ThisTask)) {

            // Start Timer
            std::chrono::time_point Start = std::chrono::high_resolution_clock::now();

            // -- Phase 1 -- //
            // Firstly, we get some important pointers out of the struct for more clear access
            size_t ShapeID = ThisTask->ShapeID_;
            float VoxelResolution_um = ThisTask->VoxelResolution_um_;
            VoxelArray* Array = ThisTask->Array_;
            std::string ShapeName = "";
            Geometries::GeometryCollection* GeometryCollection = ThisTask->GeometryCollection_;


            // -- Phase 2 -- //
            // Now, we just use the data we got from the struct and use it to call the right function
            // This sets the relevant voxels in the array
            // Note: We're not worried about synchronization here since it's okay if voxels overlap, and the voxelarray is of a static size
            // If we were to use something like a std::vector, that would be dangerous - but since we're using a static size raw array, 
            // we can allow all threads to write the array at the same time (it feels wrong, but should be okay in this specific case)
            if (GeometryCollection->IsSphere(ShapeID)) {
                Geometries::Sphere & ThisSphere = GeometryCollection->GetSphere(ShapeID);
                ShapeName = "Sphere";
                FillShape(Array, &ThisSphere, VoxelResolution_um);
            }
            else if (GeometryCollection->IsBox(ShapeID)) {
                Geometries::Box & ThisBox = GeometryCollection->GetBox(ShapeID); 
                ShapeName = "Box";
                FillBox(Array, &ThisBox, VoxelResolution_um);
            }
            else if (GeometryCollection->IsCylinder(ShapeID)) {
                Geometries::Cylinder & ThisCylinder = GeometryCollection->GetCylinder(ShapeID);
                ShapeName = "Cylinder";
                FillCylinder(Array, &ThisCylinder, VoxelResolution_um);
            }
            

            // Measure Time
            double Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
            Logger_ ->Log("ArrayGeneratorPool Thread '" + std::to_string(_ThreadNumber) + "' Processed Shape '" + ShapeName + " (" + std::to_string(ShapeID) + ")' In " + std::to_string(Duration_ms) + "ms", 0);

            // Update Task Result
            ThisTask->IsDone_ = true;

        } else {

            // We didn't get any work, just go to sleep for a few milliseconds so we don't rail the cpu
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}


// Constructor, Destructor
ArrayGeneratorPool::ArrayGeneratorPool(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads) {
    assert(_Logger != nullptr);


    // Initialize Variables
    Logger_ = _Logger;
    ThreadControlFlag_ = true;


    // Create Renderer Instances
    Logger_->Log("Creating ArrayGeneratorPool With " + std::to_string(_NumThreads) + " Thread(s)", 2);
    for (unsigned int i = 0; i < _NumThreads; i++) {
        Logger_->Log("Starting ArrayGeneratorPool Thread " + std::to_string(i), 1);
        RenderThreads_.push_back(std::thread(&ArrayGeneratorPool::RendererThreadMainFunction, this, i));
    }
}

ArrayGeneratorPool::~ArrayGeneratorPool() {

    // Send Stop Signal To Threads
    Logger_->Log("Stopping ArrayGeneratorPool Threads", 2);
    ThreadControlFlag_ = false;

    // Join All Threads
    Logger_->Log("Joining ArrayGeneratorPool Threads", 1);
    for (unsigned int i = 0; i < RenderThreads_.size(); i++) {
        Logger_->Log("Joining ArrayGeneratorPool Thread " + std::to_string(i), 0);
        RenderThreads_[i].join();
    }

}


// Queue Access Functions
void ArrayGeneratorPool::EnqueueTask(Task* _Task) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    Queue_.emplace(_Task);
}

int ArrayGeneratorPool::GetQueueSize() {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    int QueueSize = Queue_.size();

    return QueueSize;
}

bool ArrayGeneratorPool::DequeueTask(Task** _TaskPtr) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    // If the queue isn't empty, we grab the first element
    if (Queue_.size() > 0) {
        *_TaskPtr = Queue_.front();
        Queue_.pop();

        return true;
    }

    return false;
}


// Public Enqueue Function
void ArrayGeneratorPool::QueueWorkOperation(Task* _Task) {
    EnqueueTask(_Task);
}


}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG