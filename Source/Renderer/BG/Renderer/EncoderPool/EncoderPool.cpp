#include <BG/Renderer/EncoderPool/EncoderPool.h>

#include <chrono>

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
// #define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>


namespace BG {
namespace NES {
namespace Renderer {



// Thread Main Function
void EncoderPool::EncoderThreadMainFunction(int _ThreadNumber) {

    Logger_->Log("Started EncoderPool Thread " + std::to_string(_ThreadNumber), 0);


    // Run until thread exit is requested - that is, this is set to false
    while (ThreadControlFlag_) {

        // Step 1, Check For Work
        Image* ImgToProcess = nullptr;
        if (DequeueImage(&ImgToProcess)) {

            // Start Timer
            std::chrono::time_point Start = std::chrono::high_resolution_clock::now();


            // Get Image Properties
            int SourceX = ImgToProcess->Width_px;
            int SourceY = ImgToProcess->Height_px;
            int Channels = ImgToProcess->NumChannels_;
            unsigned char* SourcePixels = ImgToProcess->Data_.get();

            // Resize Image
            int TargetX = ImgToProcess->TargetWidth_px;
            int TargetY = ImgToProcess->TargetHeight_px;
            std::unique_ptr<unsigned char> ResizedPixels = std::unique_ptr<unsigned char>(new unsigned char[TargetX * TargetY * Channels]());
            stbir_resize_uint8(SourcePixels, SourceX, SourceY, SourceX * Channels, ResizedPixels.get(), TargetX, TargetY, TargetX * Channels, Channels);

            // Write Image
            stbi_write_png(ImgToProcess->TargetFileName_.c_str(), TargetX, TargetY, Channels, ResizedPixels.get(), TargetX * Channels);

            // Measure Time
            double Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
            Logger_ ->Log("EncoderPool Thread '" + std::to_string(_ThreadNumber) + "' Processed Image '" + ImgToProcess->TargetFileName_ + "' In " + std::to_string(Duration_ms) + "ms", 0);

            // Update Image Result
            ImgToProcess->ImageState_ = IMAGE_PROCESSED;

        } else {

            // We didn't get any work, just go to sleep for a few milliseconds so we don't rail the cpu
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}


// Constructor, Destructor
EncoderPool::EncoderPool(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads) {
    assert(_Logger != nullptr);


    // Initialize Variables
    Logger_ = _Logger;
    ThreadControlFlag_ = true;


    // Create Renderer Instances
    Logger_->Log("Creating EncoderPool With " + std::to_string(_NumThreads) + " Thread(s)", 2);
    for (unsigned int i = 0; i < _NumThreads; i++) {
        Logger_->Log("Starting EncoderPool Thread " + std::to_string(i), 1);
        EncoderThreads_.push_back(std::thread(&EncoderPool::EncoderThreadMainFunction, this, i));
    }
}

EncoderPool::~EncoderPool() {

    // Send Stop Signal To Threads
    Logger_->Log("Stopping EncoderPool Threads", 2);
    ThreadControlFlag_ = false;

    // Join All Threads
    Logger_->Log("Joining EncoderPool Threads", 1);
    for (unsigned int i = 0; i < EncoderThreads_.size(); i++) {
        Logger_->Log("Joining EncoderPool Thread " + std::to_string(i), 0);
        EncoderThreads_[i].join();
    }

}


// Queue Access Functions
void EncoderPool::EnqueueImage(Image* _Img) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    Queue_.emplace(_Img);
}

int EncoderPool::GetQueueSize() {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    int QueueSize = Queue_.size();

    return QueueSize;
}

bool EncoderPool::DequeueImage(Image** _ImgPtr) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    // If the queue isn't empty, we grab the first element
    if (Queue_.size() > 0) {
        *_ImgPtr = Queue_.front();
        Queue_.pop();

        return true;
    }

    return false;
}


// Public Enqueue Function
void EncoderPool::QueueEncodeOperation(Image* _Img) {
    EnqueueImage(_Img);
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG