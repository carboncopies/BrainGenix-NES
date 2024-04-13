//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>
#include <filesystem>


// Third-Party Libraries (BG convention: use <> instead of "")
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h>



namespace BG {
namespace NES {
namespace Simulator {




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
    pthread_setname_np(pthread_self(), std::string("EM Image Processor Pool Thread " + std::to_string(_ThreadNumber)).c_str());

    Logger_->Log("Started EMImageProcessorPool Thread " + std::to_string(_ThreadNumber), 0);

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

                    // -- Compositor Rules -- //
                    // In order for us to have some way that the system can repeatibly handle information, we define these rules
                    // They specify what will show up from a multilayer voxel array
                    // Firstly, we render from bottom to top - that is, from a lower Z height to a higher Z Height.
                    // Any debug colors (from bottom to top, whichever is encountered earlier), will overwrite any color in the pixels
                    // This will also terminate the continuation of enumerating up the Z height
                    // Other than those enums, we will pick the darkest color currently <--- NO WE DON'T WE JUST PICK THE TOP ONE!
                    // This isn't super realistic and needs to be fixed later, (such as with a focal distance, and blurring), but it works for now


                    // Enumerate Depth, Compose based on rules defined above
                    VoxelType PresentingVoxel;
                    uint8_t DarkestVoxel = __UINT8_MAX__;
                    for (size_t ZIndex = Task->VoxelZ; ZIndex < Task->VoxelZ + Task->SliceThickness_vox; ZIndex++) {

                        // Get Voxel At Position
                        VoxelType ThisVoxel = Task->Array_->GetVoxel(XVoxelIndex, YVoxelIndex, ZIndex);
                        if (ThisVoxel.State_ == VOXELSTATE_INTENSITY) {
                            // if (ThisVoxel.Intensity_ < DarkestVoxel) {
                                PresentingVoxel = ThisVoxel;
                                DarkestVoxel = ThisVoxel.Intensity_;
                            // }
                        // Else, we have a special debug voxel, set this to be the one shown, and exit.
                        } else {
                            PresentingVoxel = ThisVoxel;
                            break;
                        }

                    }



                    // Now Set The Pixel
                    int ThisPixelX = XVoxelIndex - Task->VoxelStartingX;
                    int ThisPixelY = YVoxelIndex - Task->VoxelStartingY;

                    // Check if the voxel is in the enum reserved range, otherwise it's a color value from 128-255
                    if (PresentingVoxel.State_ == VOXELSTATE_INTENSITY) {
                        OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, PresentingVoxel.Intensity_, PresentingVoxel.Intensity_, PresentingVoxel.Intensity_);
                        
                    } else {
                        if (PresentingVoxel.State_ == BORDER) {
                            OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 255, 128, 50);
                        } else if (PresentingVoxel.State_ == OUT_OF_RANGE) {
                            OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 0, 0, 255);
                        } else if (PresentingVoxel.State_ == VOXELSTATE_RED) {
                            OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 255, 0, 0);
                        } else if (PresentingVoxel.State_ == VOXELSTATE_GREEN) {
                            OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 0, 255, 0);
                        } else if (PresentingVoxel.State_ == VOXELSTATE_BLUE) {
                            OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 0, 0, 255);
                        } else {
                            OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 200, 200, 200);
                        }
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
            bool ResizeImage = (SourceX != TargetX) || (SourceY != TargetY);
            std::unique_ptr<unsigned char> ResizedPixels;
            if (ResizeImage) {
                ResizedPixels = std::unique_ptr<unsigned char>(new unsigned char[TargetX * TargetY * Channels]());
                stbir_resize_uint8(SourcePixels, SourceX, SourceY, SourceX * Channels, ResizedPixels.get(), TargetX, TargetY, TargetX * Channels, Channels);
            }



            // -- Phase 3 -- //
            // Now, we check that the image has a place to go, and write it to disk.

            // Ensure Path Exists
            std::error_code Code;
            if (!CreateDirectoryRecursive(Task->TargetDirectory_, Code)) {
                Logger_ ->Log("Failed To Create Directory, Error '" + Code.message() + "'", 7);
            }

            // Write Image
            unsigned char* OutPixels = SourcePixels;
            if (ResizeImage) {
                OutPixels = ResizedPixels.get();
            }
            
            stbi_write_png((Task->TargetDirectory_ + Task->TargetFileName_).c_str(), TargetX, TargetY, Channels, OutPixels, TargetX * Channels);

            // Update Task Result
            Task->IsDone_ = true;

            // Measure Time
            double Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
            Times.push_back(Duration_ms);
            if (Times.size() > SamplesBeforeUpdate) {
                double AverageTime = GetAverage(&Times);
                Logger_ ->Log("EMImageProcessorPool Thread Info '" + std::to_string(_ThreadNumber) + "' Processed Most Recent Image '" + Task->TargetFileName_ + "',  Averaging " + std::to_string(AverageTime) + "ms / Image", 0);
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
    Logger_->Log("Creating EMImageProcessorPool With " + std::to_string(_NumThreads) + " Thread(s)", 2);
    for (unsigned int i = 0; i < _NumThreads; i++) {
        Logger_->Log("Starting EMImageProcessorPool Thread " + std::to_string(i), 1);
        EncoderThreads_.push_back(std::thread(&ImageProcessorPool::EncoderThreadMainFunction, this, i));
    }
}

ImageProcessorPool::~ImageProcessorPool() {

    // Send Stop Signal To Threads
    Logger_->Log("Stopping EMImageProcessorPool Threads", 2);
    ThreadControlFlag_ = false;

    // Join All Threads
    Logger_->Log("Joining EMImageProcessorPool Threads", 1);
    for (unsigned int i = 0; i < EncoderThreads_.size(); i++) {
        Logger_->Log("Joining EMImageProcessorPool Thread " + std::to_string(i), 0);
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