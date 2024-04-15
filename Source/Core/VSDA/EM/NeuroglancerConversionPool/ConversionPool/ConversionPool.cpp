//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>
#include <filesystem>


// Third-Party Libraries (BG convention: use <> instead of "")
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
// #define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/NeuroglancerConversionPool/ConversionPool/ConversionPool.h>



namespace BG {
namespace NES {
namespace ConversionPool {




// Returns:
//   true upon success.
//   false upon failure, and set the std::error_code & err accordingly.
// https://stackoverflow.com/questions/71658440/c17-create-directories-automatically-given-a-file-path
bool CreateDirectoryRecursive(std::string const & dirName, std::error_code & err)
{
    err.clear();
    if (!std::filesystem::create_directories(dirName, err))
    {
        if (std::filesystem::exists(dirName))
        {
            // The folder already exists:
            err.clear();
            return true;    
        }
        return false;
    }
    return true;
}



// Simple little helper that just calculates the average of a double vector
double GetAverage(std::vector<double>* _Vec) {
    double Total = 0;
    for (size_t i = 0; i < _Vec->size(); i++) {
        Total += (*_Vec)[i];
    }
    return Total / _Vec->size();
}


// Thread Main Function
void ConversionPool::EncoderThreadMainFunction(int _ThreadNumber) {

    // Set thread Name
    pthread_setname_np(pthread_self(), std::string("EM Image Processor Pool Thread " + std::to_string(_ThreadNumber)).c_str());

    Logger_->Log("Started EMConversionPool Thread " + std::to_string(_ThreadNumber), 0);

    // Initialize Metrics
    int SamplesBeforeUpdate = 25;
    std::vector<double> Times;


    // Run until thread exit is requested - that is, this is set to false
    while (ThreadControlFlag_) {

        // Step 1, Check For Work
        ProcessingTask* Task = nullptr;
        if (DequeueTask(&Task)) {

            // Start Timer
            std::chrono::time_point Start = std::chrono::high_resolution_clock::now();


            // Calculate desired output filename
            std::string X1 = std::to_string(Task->IndexInfo_.StartX);
            std::string X2 = std::to_string(Task->IndexInfo_.EndX);
            std::string Y1 = std::to_string(Task->IndexInfo_.StartY);
            std::string Y2 = std::to_string(Task->IndexInfo_.EndY);
            std::string Z1 = std::to_string(Task->IndexInfo_.StartZ);
            std::string Z2 = std::to_string(Task->IndexInfo_.EndZ);
            std::string TargetFilename = Task->OutputDirectoryBasePath_ + "/" + X1 + "-" + X2;
            TargetFilename += "_" + Y1 + "-" + Y2;
            TargetFilename += "_" + Z1 + "-" + Z2;


            // Load the source image
            int Width, Height, Channels;
            unsigned char* Image = stbi_load(Task->SourceFilePath_.c_str(), &Width, &Height, &Channels, 0);

            // Now write it as a jpeg
            stbi_write_jpg(TargetFilename.c_str(), Width, Height, Channels, Image, 100);

            
            // Update Task Result
            Task->IsDone_ = true;

            // Measure Time
            double Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
            Times.push_back(Duration_ms);
            if (Times.size() > SamplesBeforeUpdate) {
                double AverageTime = GetAverage(&Times);
                Logger_ ->Log("EMConversionPool Thread Info '" + std::to_string(_ThreadNumber) + "' Processed Most Recent Image '" + TargetFilename + "',  Averaging " + std::to_string(AverageTime) + "ms / Image", 0);
                Times.clear();
            }


        } else {

            // We didn't get any work, just go to sleep for a few milliseconds so we don't rail the cpu
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}


// Constructor, Destructor
ConversionPool::ConversionPool(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads) {
    assert(_Logger != nullptr);


    // Initialize Variables
    Logger_ = _Logger;
    ThreadControlFlag_ = true;


    // Create Renderer Instances
    Logger_->Log("Creating EMConversionPool With " + std::to_string(_NumThreads) + " Thread(s)", 2);
    for (unsigned int i = 0; i < _NumThreads; i++) {
        Logger_->Log("Starting EMConversionPool Thread " + std::to_string(i), 1);
        EncoderThreads_.push_back(std::thread(&ConversionPool::EncoderThreadMainFunction, this, i));
    }
}

ConversionPool::~ConversionPool() {

    // Send Stop Signal To Threads
    Logger_->Log("Stopping EMConversionPool Threads", 2);
    ThreadControlFlag_ = false;

    // Join All Threads
    Logger_->Log("Joining EMConversionPool Threads", 1);
    for (unsigned int i = 0; i < EncoderThreads_.size(); i++) {
        Logger_->Log("Joining EMConversionPool Thread " + std::to_string(i), 0);
        EncoderThreads_[i].join();
    }

}


// Queue Access Functions
void ConversionPool::EnqueueTask(ProcessingTask* _Task) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    Queue_.emplace(_Task);
}

int ConversionPool::GetQueueSize() {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    int QueueSize = Queue_.size();

    return QueueSize;
}

bool ConversionPool::DequeueTask(ProcessingTask** _Task) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    // If the queue isn't empty, we grab the first element
    if (Queue_.size() > 0) {
        *_Task = Queue_.front();
        Queue_.pop();

        return true;
    }

    return false;
}


// Public Enqueue Function
void ConversionPool::QueueEncodeOperation(ProcessingTask* _Task) {
    EnqueueTask(_Task);
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG