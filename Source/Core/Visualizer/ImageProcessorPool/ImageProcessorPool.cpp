//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>
#include <filesystem>
#include <iostream>


// Third-Party Libraries (BG convention: use <> instead of "")
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <Visualizer/ImageProcessorPool/ImageProcessorPool.h>



namespace BG {
namespace NES {
namespace Visualizer {





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
void ImageProcessorPool::EncoderThreadMainFunction(int _ThreadNumber) {

    // Set thread Name
    pthread_setname_np(pthread_self(), std::string("Visualizer Image Processor Pool Thread " + std::to_string(_ThreadNumber)).c_str());

    Logger_->Log("Started VisualizerImageProcessorPool Thread " + std::to_string(_ThreadNumber), 0);

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


            // // Ensure Path Exists
            // std::error_code Code;
            // if (!CreateDirectoryRecursive(Task->TargetDirectory_, Code)) {
            //     Logger_ ->Log("Failed To Create Directory, Error '" + Code.message() + "'", 7);
            // }


            // Get Image Properties, Write
            BG::NES::Renderer::Image* RenderedImage = &Task->Image_;
            int SourceX = RenderedImage->Width_px;
            int SourceY = RenderedImage->Height_px;
            int Channels = RenderedImage->NumChannels_;
            unsigned char* SourcePixels = RenderedImage->Data_.get();
            int Result = stbi_write_png(Task->TargetFileName_.c_str(), SourceX, SourceY, Channels, SourcePixels, SourceX * Channels);

            // Log Metrics
            double Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
            if (Result != 0) {
                Logger_->Log("VisualizerImageEncoderPool Wrote Image '" + Task->TargetFileName_ + "' In " + std::to_string(Duration_ms) + "ms", 4);
            } else {
                Logger_->Log("VisualizerImageEncoderPool Failed to Write Image '" + Task->TargetFileName_ + "' In " + std::to_string(Duration_ms) + "ms", 9);
            }

            Task->IsDone_ = true;


        } else {

            // We didn't get any work, just go to sleep for a few milliseconds so we don't rail the cpu
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}


// Constructor, Destructor
ImageProcessorPool::ImageProcessorPool(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads) {
    assert(_Logger != nullptr);


    // Initialize Variables
    Logger_ = _Logger;
    ThreadControlFlag_ = true;


    // Create Renderer Instances
    Logger_->Log("Creating VisualizerImageProcessorPool With " + std::to_string(_NumThreads) + " Thread(s)", 2);
    for (unsigned int i = 0; i < _NumThreads; i++) {
        Logger_->Log("Starting VisualizerImageProcessorPool Thread " + std::to_string(i), 1);
        EncoderThreads_.push_back(std::thread(&ImageProcessorPool::EncoderThreadMainFunction, this, i));
    }
}

ImageProcessorPool::~ImageProcessorPool() {

    // Send Stop Signal To Threads
    Logger_->Log("Stopping VisualizerImageProcessorPool Threads", 2);
    ThreadControlFlag_ = false;

    // Join All Threads
    Logger_->Log("Joining VisualizerImageProcessorPool Threads", 1);
    for (unsigned int i = 0; i < EncoderThreads_.size(); i++) {
        Logger_->Log("Joining VisualizerImageProcessorPool Thread " + std::to_string(i), 0);
        EncoderThreads_[i].join();
    }

}


// Queue Access Functions
void ImageProcessorPool::EnqueueTask(ProcessingTask* _Task) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);
    Queue_.emplace(_Task);
}

int ImageProcessorPool::GetQueueSize() {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    int QueueSize = Queue_.size();

    return QueueSize;
}

bool ImageProcessorPool::DequeueTask(ProcessingTask** _Task) {

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
void ImageProcessorPool::QueueEncodeOperation(ProcessingTask* _Task) {
    EnqueueTask(_Task);
}



}; // Close Namespace VSDA
}; // Close Namespace NES
}; // Close Namespace BG
