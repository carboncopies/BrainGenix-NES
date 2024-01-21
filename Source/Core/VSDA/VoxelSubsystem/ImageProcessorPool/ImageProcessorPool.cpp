//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h>



namespace BG {
namespace NES {
namespace Simulator {



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
    pthread_setname_np(pthread_self(), std::string("Image Processor Pool Thread " + std::to_string(_ThreadNumber)).c_str());

    Logger_->Log("Started ImageProcessorPool Thread " + std::to_string(_ThreadNumber), 0);

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


            // -- Phase 1 -- //

            // First, setup the 1:1 voxel array image base and get it ready to be drawn to
            // If the user wants for example, 8 pixels per voxel (8x8), then we make an image 1/8 the dimensions as desired
            // then we set each pixel here based on the voxel in the map
            // next, we resize it up to the target image, thus saving a lot of compute time
            int VoxelsPerStepX = Task->VoxelEndingX - Task->VoxelStartingX;
            int VoxelsPerStepY = Task->VoxelEndingY - Task->VoxelStartingY;
            int NumChannels = 3;

            Image OneToOneVoxelImage(VoxelsPerStepX, VoxelsPerStepY, NumChannels);
            OneToOneVoxelImage.TargetFileName_ = Task->TargetFileName_;

            // Now enumerate the voxel array and populate the image with the desired pixels (for the subregion we're on)
            for (unsigned int XVoxelIndex = Task->VoxelStartingX; XVoxelIndex < Task->VoxelEndingX; XVoxelIndex++) {
                for (unsigned int YVoxelIndex = Task->VoxelStartingY; YVoxelIndex < Task->VoxelEndingY; YVoxelIndex++) {

                    // Get Voxel At Position
                    VoxelType ThisVoxel = Task->Array_->GetVoxel(XVoxelIndex, YVoxelIndex, Task->VoxelZ);

                    // Now Set The Pixel
                    int ThisPixelX = XVoxelIndex - Task->VoxelStartingX;
                    int ThisPixelY = YVoxelIndex - Task->VoxelStartingY;
                    if (ThisVoxel == FILLED) {
                        OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 220, 220, 220);
                    } else if (ThisVoxel == BORDER) {
                        OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 255, 128, 50);
                    } else if (ThisVoxel == OUT_OF_RANGE) {
                        OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 0, 0, 255);
                    } else {
                        OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 80, 80, 80);
                    }

                }
            }

            // Note, when we do image processing (for like noise and that stuff, we should do it here!) (or after resizing depending on what is needed)
            // so then this will be phase two, and phase 3 is saving after processing


            // -- Phase 2 -- //

            // Now, we resize the image to the desired output resolution
            // Get Image Properties
            int SourceX = OneToOneVoxelImage.Width_px;
            int SourceY = OneToOneVoxelImage.Height_px;
            int Channels = OneToOneVoxelImage.NumChannels_;
            unsigned char* SourcePixels = OneToOneVoxelImage.Data_.get();

            // Resize Image
            int TargetX = Task->Width_px;
            int TargetY = Task->Height_px;
            std::unique_ptr<unsigned char> ResizedPixels = std::unique_ptr<unsigned char>(new unsigned char[TargetX * TargetY * Channels]());
            stbir_resize_uint8(SourcePixels, SourceX, SourceY, SourceX * Channels, ResizedPixels.get(), TargetX, TargetY, TargetX * Channels, Channels);

            // Write Image
            stbi_write_png(Task->TargetFileName_.c_str(), TargetX, TargetY, Channels, ResizedPixels.get(), TargetX * Channels);

            // Update Task Result
            Task->IsDone_ = true;

            // Measure Time
            double Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
            Times.push_back(Duration_ms);
            if (Times.size() > SamplesBeforeUpdate) {
                double AverageTime = GetAverage(&Times);
                Logger_ ->Log("ImageProcessorPool Thread Info '" + std::to_string(_ThreadNumber) + "' Processed Most Recent Image '" + Task->TargetFileName_ + "',  Averaging " + std::to_string(AverageTime) + "ms / Image", 0);
                Times.clear();
            }


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
    Logger_->Log("Creating ImageProcessorPool With " + std::to_string(_NumThreads) + " Thread(s)", 2);
    for (unsigned int i = 0; i < _NumThreads; i++) {
        Logger_->Log("Starting ImageProcessorPool Thread " + std::to_string(i), 1);
        EncoderThreads_.push_back(std::thread(&ImageProcessorPool::EncoderThreadMainFunction, this, i));
    }
}

ImageProcessorPool::~ImageProcessorPool() {

    // Send Stop Signal To Threads
    Logger_->Log("Stopping ImageProcessorPool Threads", 2);
    ThreadControlFlag_ = false;

    // Join All Threads
    Logger_->Log("Joining ImageProcessorPool Threads", 1);
    for (unsigned int i = 0; i < EncoderThreads_.size(); i++) {
        Logger_->Log("Joining ImageProcessorPool Thread " + std::to_string(i), 0);
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


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG