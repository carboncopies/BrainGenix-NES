//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <noise/noise.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>
#include <VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.h>





namespace BG {
namespace NES {
namespace Simulator {
namespace VoxelArrayGenerator {



// Simple little helper that just calculates the average of a double vector
double GetAverage(std::vector<double>* _Vec) {
    double Total = 0;
    for (size_t i = 0; i < _Vec->size(); i++) {
        Total += (*_Vec)[i];
    }
    return Total / _Vec->size();
}


// Thread Main Function
void ArrayGeneratorPool::RendererThreadMainFunction(int _ThreadNumber) {

    // Set thread Name
    pthread_setname_np(pthread_self(), std::string("EM Array Generator Pool Thread " + std::to_string(_ThreadNumber)).c_str());

    Logger_->Log("Started EMArrayGeneratorPool Thread " + std::to_string(_ThreadNumber), 0);


    // Setup Noise Generator
    noise::module::Perlin PerlinGenerator;

    // Initialize Metrics
    int SamplesBeforeUpdate = 10000;
    std::vector<double> Times;

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
            float VoxelResolution_um = ThisTask->WorldInfo_.VoxelScale_um;
            VoxelArray* Array = ThisTask->Array_;
            std::string ShapeName = "";
            Geometries::GeometryCollection* GeometryCollection = ThisTask->GeometryCollection_;
            assert(ThisTask->Parameters_ != nullptr);


            // -- Phase 2 -- //
            // Now, we just use the data we got from the struct and use it to call the right function
            // This sets the relevant voxels in the array
            // Note: We're not worried about synchronization here since it's okay if voxels overlap, and the voxelarray is of a static size
            // If we were to use something like a std::vector, that would be dangerous - but since we're using a static size raw array, 
            // we can allow all threads to write the array at the same time (it feels wrong, but should be okay in this specific case)
            std::string ShapeInfo;
            if (ThisTask->CustomShape_ == CUSTOM_WEDGE) {
                FillWedge(Array, &ThisTask->ThisWedge, ThisTask->WorldInfo_, ThisTask->Parameters_, &PerlinGenerator);
                ShapeName = "Wedge";
            } else if (ThisTask->CustomShape_ == CUSTOM_NONE) {
                if (GeometryCollection->IsSphere(ShapeID)) {
                    Geometries::Sphere & ThisSphere = GeometryCollection->GetSphere(ShapeID);
                    ShapeInfo += "Radius: " + std::to_string(ThisSphere.Radius_um);
                    ShapeInfo += ", X: " + std::to_string(ThisSphere.Center_um.x);
                    ShapeInfo += ", Y: " + std::to_string(ThisSphere.Center_um.y);
                    ShapeInfo += ", Z: " + std::to_string(ThisSphere.Center_um.z);
                    ShapeName = "Sphere";
                    FillSpherePart(1, 0, Array, &ThisSphere, ThisTask->WorldInfo_, ThisTask->Parameters_, &PerlinGenerator);
                }
                else if (GeometryCollection->IsBox(ShapeID)) {
                    Geometries::Box & ThisBox = GeometryCollection->GetBox(ShapeID); 
                    ShapeName = "Box";
                    FillBox(Array, &ThisBox, ThisTask->WorldInfo_, ThisTask->Parameters_, &PerlinGenerator);
                }
                else if (GeometryCollection->IsCylinder(ShapeID)) {
                    Geometries::Cylinder & ThisCylinder = GeometryCollection->GetCylinder(ShapeID);
                    ShapeName = "Cylinder";
                    //FillCylinder(Array, &ThisCylinder, ThisTask->WorldInfo_, ThisTask->Parameters_, &PerlinGenerator);
                    FillCylinderPart(1, 0, Array, &ThisCylinder, ThisTask->WorldInfo_, ThisTask->Parameters_, &PerlinGenerator);
                }
            } else {

                if (ThisTask->CustomShape_ == CUSTOM_CYLINDER) {
                    ShapeName = "CylinderPart";
                    FillCylinderPart(ThisTask->CustomTotalComponents, ThisTask->CustomThisComponent, Array, &ThisTask->CustomCylinder_, ThisTask->WorldInfo_, ThisTask->Parameters_, &PerlinGenerator);
                } else if (ThisTask->CustomShape_ == CUSTOM_SPHERE) {
                    ShapeName = "SpherePart";
                    FillSpherePart(ThisTask->CustomTotalComponents, ThisTask->CustomThisComponent, Array, &ThisTask->CustomSphere_, ThisTask->WorldInfo_, ThisTask->Parameters_, &PerlinGenerator);

                }
            }
            
            ShapeInfo = "[Type: " + ShapeName + ", " + ShapeInfo + "]";

            // Update Task Result
            ThisTask->IsDone_ = true;


            // Measure Time
            double Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
            if (Duration_ms > 2000) {
                Logger_ ->Log("EMArrayGeneratorPool Slow Shape " + std::to_string(Duration_ms) + "ms For Shape " + ShapeInfo + " (" + std::to_string(ShapeID) + ")'", 7);

            }
            Times.push_back(Duration_ms);
            if (Times.size() > SamplesBeforeUpdate) {
                double AverageTime = GetAverage(&Times);
                Logger_ ->Log("EMArrayGeneratorPool Thread Info '" + std::to_string(_ThreadNumber) + "' Processed Most Recent Shape '" + ShapeName + " (" + std::to_string(ShapeID) + ")', Averaging " + std::to_string(AverageTime) + "ms / Shape", 0);
                Times.clear();
            }



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

    // Reduced threaidng mode for debugging
    #ifdef REDUCED_THREADING_DEBUG
        _NumThreads = 1;
    #endif

    // Create Renderer Instances
    Logger_->Log("Creating EMArrayGeneratorPool With " + std::to_string(_NumThreads) + " Thread(s)", 2);
    for (unsigned int i = 0; i < _NumThreads; i++) {
        Logger_->Log("Starting EMArrayGeneratorPool Thread " + std::to_string(i), 1);
        RenderThreads_.push_back(std::thread(&ArrayGeneratorPool::RendererThreadMainFunction, this, i));
    }
}

ArrayGeneratorPool::~ArrayGeneratorPool() {

    // Send Stop Signal To Threads
    Logger_->Log("Stopping EMArrayGeneratorPool Threads", 2);
    ThreadControlFlag_ = false;

    // Join All Threads
    Logger_->Log("Joining EMArrayGeneratorPool Threads", 1);
    for (unsigned int i = 0; i < RenderThreads_.size(); i++) {
        Logger_->Log("Joining EMArrayGeneratorPool Thread " + std::to_string(i), 0);
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


std::vector<Task*> ArrayGeneratorPool::DequeueTasks(int _NumTasks) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    // If the queue isn't empty, we grab the first element
    std::vector<Task*> Tasks;
    for (unsigned int i = 0; i < _NumTasks; i++) {
        if (Queue_.size() > 0) {
            Tasks.push_back(Queue_.front());
            Queue_.pop();
        }
    }
    return Tasks;
}


// Public Enqueue Function
void ArrayGeneratorPool::QueueWorkOperation(Task* _Task) {
    EnqueueTask(_Task);
}

// Public Blocking/Info Function
void ArrayGeneratorPool::BlockUntilQueueEmpty(bool _LogOutput) {

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
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        // Log Queue Size
        Logger_->Log("EMArrayGeneratorPool Queue Length '" + std::to_string(QueueLength) + "'", 1, _LogOutput);

    }

}

}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG