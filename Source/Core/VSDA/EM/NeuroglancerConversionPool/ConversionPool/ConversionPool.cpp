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
            std::string TargetFilename;


            // Write segmentation map data
            if (Task->IsSegmentation_) {

                // Update the indexes to the scaled size
                int ScaledX1 = Task->IndexInfo_.StartX;
                int ScaledX2 = Task->IndexInfo_.EndX;
                int ScaledY1 = Task->IndexInfo_.StartY;
                int ScaledY2 = Task->IndexInfo_.EndY;
                int ScaledZ1 = Task->IndexInfo_.StartZ;
                int ScaledZ2 = Task->IndexInfo_.EndZ;

                // Calculate desired output filename
                std::string X1 = std::to_string(ScaledX1);
                std::string X2 = std::to_string(ScaledX2);
                std::string Y1 = std::to_string(ScaledY1);
                std::string Y2 = std::to_string(ScaledY2);
                std::string Z1 = std::to_string(ScaledZ1);
                std::string Z2 = std::to_string(ScaledZ2);
                TargetFilename = Task->OutputDirectoryBasePath_ + "Segmentation";
                TargetFilename += "/" + X1 + "-" + X2;
                TargetFilename += "_" + Y1 + "-" + Y2;
                TargetFilename += "_" + Z1 + "-" + Z2;

                // Copy file
                try {
                    std::filesystem::copy_file(Task->SourceFilePath_, TargetFilename); 
                } catch (const std::filesystem::filesystem_error& e) {
                } 

            } else {
                // Load the source image
                int Width, Height, Channels;
                unsigned char* Image = stbi_load(Task->SourceFilePath_.c_str(), &Width, &Height, &Channels, 0);

                for (int ReductionLevel = 0; ReductionLevel <= Task->ReductionLevels_; ReductionLevel++) {

                    // Calculate the new dimensions
                    int NewWidth = Width / pow(2,ReductionLevel);
                    int NewHeight = Height / pow(2,ReductionLevel);

                    // Allocate memory for the resized image
                    unsigned char* ResizedImage = (unsigned char*)malloc(NewWidth * NewHeight * Channels);

                    // Resize the image
                    stbir_resize_uint8(Image, Width, Height, 0, ResizedImage, NewWidth, NewHeight, 0, Channels);

                    // Update the indexes to the scaled size
                    int ScaledX1 = Task->IndexInfo_.StartX / pow(2,ReductionLevel);
                    int ScaledX2 = Task->IndexInfo_.EndX / pow(2,ReductionLevel);
                    int ScaledY1 = Task->IndexInfo_.StartY / pow(2,ReductionLevel);
                    int ScaledY2 = Task->IndexInfo_.EndY / pow(2,ReductionLevel);
                    int ScaledZ1 = Task->IndexInfo_.StartZ;
                    int ScaledZ2 = Task->IndexInfo_.EndZ;

                    // Calculate desired output filename
                    std::string X1 = std::to_string(ScaledX1);
                    std::string X2 = std::to_string(ScaledX2);
                    std::string Y1 = std::to_string(ScaledY1);
                    std::string Y2 = std::to_string(ScaledY2);
                    std::string Z1 = std::to_string(ScaledZ1);
                    std::string Z2 = std::to_string(ScaledZ2);
                    TargetFilename = Task->OutputDirectoryBasePath_ + "/ReductionLevel-" + std::to_string(ReductionLevel);
                    TargetFilename += "/" + X1 + "-" + X2;
                    TargetFilename += "_" + Y1 + "-" + Y2;
                    TargetFilename += "_" + Z1 + "-" + Z2;

                    // Write the resized image as a JPEG
                    stbi_write_jpg(TargetFilename.c_str(), NewWidth, NewHeight, Channels, ResizedImage, 100);

                    // Free the resized image memory
                    free(ResizedImage);
                }

                stbi_image_free(Image);
            }

            // Update Task Result
            Task->IsDone_ = true;

            // Measure Time
            double Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
            Times.push_back(Duration_ms);
            if (Times.size() > SamplesBeforeUpdate) {
                double AverageTime = GetAverage(&Times);
                Logger_->Log("EMConversionPool Thread Info '" + std::to_string(_ThreadNumber) + "' Processed Most Recent Image '" + TargetFilename + "',  Averaging " + std::to_string(AverageTime) + "ms / Image", 0);
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

    // Reduced threaidng mode for debugging
    #ifdef REDUCED_THREADING_DEBUG
        _NumThreads = 1;
    #endif


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