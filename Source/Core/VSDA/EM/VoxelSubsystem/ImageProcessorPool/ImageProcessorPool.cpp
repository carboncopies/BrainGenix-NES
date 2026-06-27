//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>
#include <filesystem>
#include <cstdlib>
#include <cstring>
#include <random>
#include <algorithm>
#include <math.h>

#ifdef __APPLE__
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/AppleGaussianBlur.h>
#endif


// Third-Party Libraries (BG convention: use <> instead of "")
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

#define IIR_GAUSS_BLUR_IMPLEMENTATION
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/iir_gauss_blur.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h>



namespace BG {
namespace NES {
namespace Simulator {

float LinearInterpolate(float _X, float _Val1, float _Val2) {
    return _Val1 + _X * (_Val2 - _Val1);
}

uint8_t CalculateBorderColor(uint8_t _Source, float _DistanceFromEdge, MicroscopeParameters* _Params) {

    if (_DistanceFromEdge < _Params->BorderThickness_um) {

        float NormalizedDistanceFromEdge = 1.0f - (_DistanceFromEdge / _Params->BorderThickness_um);
        return LinearInterpolate(NormalizedDistanceFromEdge, _Source, _Params->BorderEdgeIntensity);
    }

    return _Source;

}

uint8_t GenerateVoxelColor(float _X_um, float _Y_um, float _Z_um, MicroscopeParameters* _Params, noise::module::Perlin* _Generator, int _Offset=0) {

    // Now, generate the color based on some noise constraints, Clamp it between 0 and 1, then scale based on parameters
    double NoiseValue;
    if (_Params->GeneratePerlinNoise_) {
        float SpatialScale = _Params->SpatialScale_;
        NoiseValue = _Generator->GetValue(_X_um * SpatialScale, _Y_um * SpatialScale, _Z_um * SpatialScale);
        NoiseValue = (NoiseValue / 2.) + 0.5;
        NoiseValue *= _Params->NoiseIntensity_;
    }

    double VoxelColorValue = _Params->DefaultIntensity_ - NoiseValue;
    VoxelColorValue += _Offset;
    VoxelColorValue = std::min(255., VoxelColorValue);
    VoxelColorValue = std::max(0., VoxelColorValue);





    return VoxelColorValue;

}


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

inline uint64_t GetVoxelBufferIndex(int x, int y, int z, uint64_t yz_stride, uint64_t z_stride) {
    return (uint64_t(x) * yz_stride) + (uint64_t(y) * z_stride) + uint64_t(z);
}


// Thread Main Function
void ImageProcessorPool::EncoderThreadMainFunction(int _ThreadNumber) {

    // Set thread Name
    #ifdef __APPLE__
    pthread_setname_np(std::string("EM Image Processor Pool Thread " + std::to_string(_ThreadNumber)).c_str());
#else
    pthread_setname_np(pthread_self(), std::string("EM Image Processor Pool Thread " + std::to_string(_ThreadNumber)).c_str());
#endif

    Logger_->Log("Started EMImageProcessorPool Thread " + std::to_string(_ThreadNumber), 0);

    // Initialize Metrics
    int SamplesBeforeUpdate = 2500;
    std::vector<double> Times;

    std::mt19937 RandomGenerator(rand());

    // Run until thread exit is requested - that is, this is set to false
    while (ThreadControlFlag_) {

        // Step 1, Check For Work
        ProcessingTask* Task = nullptr;
        if (DequeueTask(&Task)) {
            auto CompleteTask = [this, Task]() {
                Task->IsDone_ = true;
                ActiveTasks_.fetch_sub(1, std::memory_order_release);
                IdleCondition_.notify_all();
            };

            // If we're compressing instead.
            if (Task->IsSegmentation_) {
                // Handle segmentation compression
                SegmentationCompressor::ProcessTask(Task);

                // toggle this to disable writing colored pngs
                if (Task->Params_->GenerateSegmentationPNGs) {
                    
                    // -- Phase 1 -- //

                    // First, setup the 1:1 voxel array image base and get it ready to be drawn to
                    // If the user wants for example, 8 pixels per voxel (8x8), then we make an image 1/8 the dimensions as desired
                    // then we set each pixel here based on the voxel in the map
                    // next, we resize it up to the target image, thus saving a lot of compute time
                    int VoxelsPerStepX = Task->VoxelEndingX - Task->VoxelStartingX;
                    int VoxelsPerStepY = Task->VoxelEndingY - Task->VoxelStartingY;
                    int NumChannels = 3;
                    const int array_y = Task->Array_->GetY();
                    const int array_z = Task->Array_->GetZ();
                    const uint64_t yz_stride = uint64_t(array_y) * uint64_t(array_z);
                    const uint64_t z_stride = uint64_t(array_z);
                    const VoxelType* voxel_data = Task->Array_->GetRawData();

                    Image OneToOneVoxelImage(VoxelsPerStepX, VoxelsPerStepY, NumChannels);
                    OneToOneVoxelImage.TargetFileName_ = Task->TargetFileName_;
                    unsigned char* image_data = OneToOneVoxelImage.Data_.get();

                    // Now enumerate the voxel array and populate the image with the desired pixels (for the subregion we're on)
                    for (unsigned int XVoxelIndex = Task->VoxelStartingX; XVoxelIndex < Task->VoxelEndingX; XVoxelIndex++) {
                        const int pixel_x = XVoxelIndex - Task->VoxelStartingX;
                        const uint64_t x_base = GetVoxelBufferIndex(XVoxelIndex, 0, Task->VoxelZ, yz_stride, z_stride);
                        for (unsigned int YVoxelIndex = Task->VoxelStartingY; YVoxelIndex < Task->VoxelEndingY; YVoxelIndex++) {
                            const VoxelType& PresentingVoxel = voxel_data[x_base + (uint64_t(YVoxelIndex) * z_stride)];
                            const int pixel_y = YVoxelIndex - Task->VoxelStartingY;
                            const size_t pixel_offset = (pixel_x + (VoxelsPerStepX * pixel_y)) * NumChannels;

                            uint64_t Seed = PresentingVoxel.ParentUID;
                            unsigned int R = (Seed * 9301 + 49297) % 256;
                            unsigned int G = (Seed * 8303 + 49299) % 256;
                            unsigned int B = (Seed * 7307 + 49303) % 256;

                            if ((Seed == 0) || (PresentingVoxel.State_ == VoxelState_EMPTY) || (PresentingVoxel.State_ == VoxelState_OUT_OF_BOUNDS)) {
                                R = 0;
                                G = 0;
                                B = 0;
                            }

                            image_data[pixel_offset + 0] = static_cast<unsigned char>(R);
                            image_data[pixel_offset + 1] = static_cast<unsigned char>(G);
                            image_data[pixel_offset + 2] = static_cast<unsigned char>(B);
                                

                        }
                    }


                    int SourceX = OneToOneVoxelImage.Width_px;
                    int SourceY = OneToOneVoxelImage.Height_px;
                    int Channels = OneToOneVoxelImage.NumChannels_;
                    unsigned char* SourcePixels = OneToOneVoxelImage.Data_.get();


                    // Ensure Path Exists
                    std::error_code Code;
                    if (!CreateDirectoryRecursive(Task->TargetDirectory_, Code)) {
                        Logger_ ->Log("Failed To Create Directory, Error '" + Code.message() + "'", 7);
                    }

                    // Write Image
                    stbi_write_png((Task->TargetDirectory_ + Task->TargetFileName_ + ".seg.png").c_str(), SourceX, SourceY, Channels, SourcePixels, SourceX * Channels);

                }

                CompleteTask();

                // Logging
                //Logger_->Log("Compressed segmentation layer " + std::to_string(Task->VoxelZ) + " to " + Task->OutputPath_, 1);
                continue;
            }

            
            // Start Timer
            std::chrono::time_point Start = std::chrono::high_resolution_clock::now();


            // -- Phase 1 -- //

            // First, setup the 1:1 voxel array image base and get it ready to be drawn to
            // If the user wants for example, 8 pixels per voxel (8x8), then we make an image 1/8 the dimensions as desired
            // then we set each pixel here based on the voxel in the map
            // next, we resize it up to the target image, thus saving a lot of compute time
            int VoxelsPerStepX = Task->VoxelEndingX - Task->VoxelStartingX;
            int VoxelsPerStepY = Task->VoxelEndingY - Task->VoxelStartingY;
            int NumChannels = 1;
            const int array_y = Task->Array_->GetY();
            const int array_z = Task->Array_->GetZ();
            const uint64_t yz_stride = uint64_t(array_y) * uint64_t(array_z);
            const uint64_t z_stride = uint64_t(array_z);
            const VoxelType* voxel_data = Task->Array_->GetRawData();
            const BoundingBox voxel_bb = Task->Array_->GetBoundingBox();
            const float voxel_scale = Task->VoxelScale_um;
            const float z_position_um = voxel_bb.bb_point1[2] + (Task->VoxelZ * voxel_scale);

            Image OneToOneVoxelImage(VoxelsPerStepX, VoxelsPerStepY, NumChannels);
            OneToOneVoxelImage.TargetFileName_ = Task->TargetFileName_;
            unsigned char* image_data = OneToOneVoxelImage.Data_.get();
            std::fill_n(image_data, VoxelsPerStepX * VoxelsPerStepY, static_cast<unsigned char>(240));

            // Now enumerate the voxel array and populate the image with the desired pixels (for the subregion we're on)
            bool IsImageEmpty = true;
            for (unsigned int XVoxelIndex = Task->VoxelStartingX; XVoxelIndex < Task->VoxelEndingX; XVoxelIndex++) {
                const int pixel_x = XVoxelIndex - Task->VoxelStartingX;
                const uint64_t x_base = GetVoxelBufferIndex(XVoxelIndex, 0, Task->VoxelZ, yz_stride, z_stride);
                const float x_position_um = voxel_bb.bb_point1[0] + (XVoxelIndex * voxel_scale);
                for (unsigned int YVoxelIndex = Task->VoxelStartingY; YVoxelIndex < Task->VoxelEndingY; YVoxelIndex++) {
                    const VoxelType& PresentingVoxel = voxel_data[x_base + (uint64_t(YVoxelIndex) * z_stride)];
                    const int pixel_y = YVoxelIndex - Task->VoxelStartingY;
                    const size_t pixel_offset = pixel_x + (VoxelsPerStepX * pixel_y);

                    // Calculate Color To Be Set
                    if (PresentingVoxel.State_ == VoxelState_BLACK) {
                        image_data[pixel_offset] = 0;
                        IsImageEmpty = false;
                        continue;
                    } else if (PresentingVoxel.State_ == VoxelState_WHITE) {
                        image_data[pixel_offset] = 255;
                        IsImageEmpty = false;
                        continue;
                    } else if (PresentingVoxel.State_ == VoxelState_EMPTY) {
                        continue;                    
                    } else if (PresentingVoxel.State_ == VoxelState_OUT_OF_BOUNDS) {
                        image_data[pixel_offset] = 0; // Force out of bounds color to be black.
                        continue;                    
                    } 

                    // If we've gotten this far, the voxel must be inside something
                    // then we set the color based on the perlin noise, and distance to edge
                    uint8_t Intensity;
                    if (Task->Params_->GeneratePerlinNoise_) {
                        const float y_position_um = voxel_bb.bb_point1[1] + (YVoxelIndex * voxel_scale);
                        Intensity = GenerateVoxelColor(x_position_um, y_position_um, z_position_um, Task->Params_, Task->Generator_);
                    } else {
                        Intensity = Task->Params_->DefaultIntensity_;
                    }

                    if (Task->Params_->RenderBorders) {
                        Intensity = CalculateBorderColor(Intensity, PresentingVoxel.DistanceToEdge_vox_, Task->Params_);
                    }

                    image_data[pixel_offset] = Intensity;
                    IsImageEmpty = false;
                        

                }
            }



            // Check for image being empty, if it is we use the null image, and dont bother finishing the render process
            if (IsImageEmpty) {

                // Ensure Path Exists
                std::error_code Code;
                if (!CreateDirectoryRecursive(Task->TargetDirectory_, Code)) {
                    Logger_ ->Log("Failed To Create Directory, Error '" + Code.message() + "'", 7);
                }

                // Copy null image into new image.
                std::filesystem::copy_file(Task->NullImagePath_, Task->TargetDirectory_ + Task->TargetFileName_, std::filesystem::copy_options::overwrite_existing);

                // Update Task Result
                CompleteTask();

                continue;
            }



            // Note, when we do image processing (for like noise and that stuff, we should do it here!) (or after resizing depending on what is needed)
            // so then this will be phase two, and phase 3 is saving after processing




            // Contrast/Brightness Adjustments
            if (Task->AdjustContrast) {
                
                // Calculate the contrast/brightness for this image
                float ThisContrast = Task->Contrast + ((-1+2*((float)rand())/RAND_MAX) * Task->ContrastRandomAmount);
                float ThisBrightness = Task->Brightness + ((-1+2*((float)rand())/RAND_MAX) * Task->BrightnessRandomAmount);

                const size_t pixel_count = size_t(OneToOneVoxelImage.Width_px) * size_t(OneToOneVoxelImage.Height_px);
                for (size_t pixel_index = 0; pixel_index < pixel_count; ++pixel_index) {
                    int Color = image_data[pixel_index];
                    Color = (ThisContrast * ((float)Color - 128.)) + 128 + ThisBrightness;
                    Color = std::clamp(Color, 0, 255);
                    image_data[pixel_index] = static_cast<unsigned char>(Color);
                }
            }

            // Add Interference pattern
            if (Task->EnableInterferencePattern) {

                // We need to calculate the random amount for this image
                float ThisImageAmplitude = (1. + (Task->InterferencePatternStrengthVariation * -1+2*((float)rand())/RAND_MAX)) * Task->InterferencePatternAmplitude;

                // Randomize the interference patterns between layers (so they don't line up between layers evenly)
                std::mt19937 ZIndexOffset(Task->VoxelZ);
                float ZOffset = (ZIndexOffset() % 5000000) / 500000;
                if (!Task->InterferencePatternZOffsetShift) {
                    ZOffset = 0;
                }

                for (size_t X = 0; X < OneToOneVoxelImage.Width_px; X++) {
                    const float PositionX = (Task->VoxelStartingX + X) * Task->VoxelScale_um;
                    for (size_t Y = 0; Y < OneToOneVoxelImage.Height_px; Y++) {
                        const size_t pixel_offset = X + (OneToOneVoxelImage.Width_px * Y);
                        int Color = image_data[pixel_offset];
                        float PositionY = (Task->VoxelStartingY + Y) * Task->VoxelScale_um;
                        float ScaledPositionX = (PositionX + ZOffset) + (sin(PositionY * Task->InterferencePatternWobbleFrequency) * Task->InterferencePatternYAxisWobbleIntensity); 
                        Color += sin(Task->InterferencePatternXScale_um * ScaledPositionX) * ThisImageAmplitude + Task->InterferencePatternBias;
                        Color = std::clamp(Color, 0, 255);
                        image_data[pixel_offset] = static_cast<unsigned char>(Color);
                    }
                }
            }


            // Pre-Blur Noise Pass
            if (Task->EnableImageNoise) {
                for (unsigned int i = 0; i < Task->PreBlurNoisePasses; i++) {
                    const size_t pixel_count = size_t(OneToOneVoxelImage.Width_px) * size_t(OneToOneVoxelImage.Height_px);
                    for (size_t pixel_index = 0; pixel_index < pixel_count; ++pixel_index) {
                        int Color = image_data[pixel_index];
                        Color += (RandomGenerator() % Task->ImageNoiseAmount) - int(Task->ImageNoiseAmount/2);
                        Color = std::clamp(Color, 0, 255);
                        image_data[pixel_index] = static_cast<unsigned char>(Color);
                    }
                }
            }

            // Perform Gaussian Blurring Step
            if (Task->EnableGaussianBlur) {
#ifdef __APPLE__
                // Tiered blur strategy on Apple Silicon:
                //   sigma >= 5 → MPS (GPU, ~10-20× faster for large blur radii)
                //   sigma <  5 → vImage tent filter (AMX SIMD, low-overhead for small radii)
                // MPS path falls back to vImage if the Metal device is unavailable.
                bool BlurDone = false;
                if (Task->GaussianBlurSigma >= 5.0f) {
                    BlurDone = MPS_GaussianBlur(
                        OneToOneVoxelImage.Data_.get(),
                        OneToOneVoxelImage.Width_px,
                        OneToOneVoxelImage.Height_px,
                        Task->GaussianBlurSigma);
                }
                if (!BlurDone) {
                    Accelerate_GaussianBlur(
                        OneToOneVoxelImage.Data_.get(),
                        OneToOneVoxelImage.Width_px,
                        OneToOneVoxelImage.Height_px,
                        Task->GaussianBlurSigma);
                }
#else
                iir_gauss_blur(OneToOneVoxelImage.Width_px, OneToOneVoxelImage.Height_px, 1, OneToOneVoxelImage.Data_.get(), Task->GaussianBlurSigma);
#endif
            }


            // Post-Blur Noise Pass
            if (Task->EnableImageNoise) {
                for (unsigned int i = 0; i < Task->PostBlurNoisePasses; i++) {
                    const size_t pixel_count = size_t(OneToOneVoxelImage.Width_px) * size_t(OneToOneVoxelImage.Height_px);
                    for (size_t pixel_index = 0; pixel_index < pixel_count; ++pixel_index) {
                        int Color = image_data[pixel_index];
                        Color += (RandomGenerator() % Task->ImageNoiseAmount) - int(Task->ImageNoiseAmount/2);
                        Color = std::clamp(Color, 0, 255);
                        image_data[pixel_index] = static_cast<unsigned char>(Color);
                    }
                }
            }



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
                stbir_resize_uint8_linear(SourcePixels, SourceX, SourceY, SourceX * Channels, ResizedPixels.get(), TargetX, TargetY, TargetX * Channels, (stbir_pixel_layout)Channels);
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
            CompleteTask();

            // Measure Time
            double Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
            Times.push_back(Duration_ms);
            if (Times.size() > SamplesBeforeUpdate) {
                double AverageTime = GetAverage(&Times);
                Logger_ ->Log("EMImageProcessorPool Thread Info '" + std::to_string(_ThreadNumber) + "' Processed Most Recent Image '" + Task->TargetFileName_ + "',  Averaging " + std::to_string(AverageTime) + "ms / Image", 0);
                Times.clear();
            }


        } else {

            std::unique_lock<std::mutex> Lock(QueueMutex_);
            WorkAvailable_.wait_for(Lock, std::chrono::milliseconds(50),
                [this]{ return !Queue_.empty() || !ThreadControlFlag_; });
        }
    }
}


// Constructor, Destructor
ImageProcessorPool::ImageProcessorPool(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads) {
    assert(_Logger != nullptr);


    // Initialize Variables
    Logger_ = _Logger;
    ThreadControlFlag_ = true;


    // Reduced threaidng mode for debugging
    #ifdef REDUCED_THREADING_DEBUG
        _NumThreads = 1;
    #endif

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
    WorkAvailable_.notify_all();

    // Join All Threads
    Logger_->Log("Joining EMImageProcessorPool Threads", 1);
    for (unsigned int i = 0; i < EncoderThreads_.size(); i++) {
        Logger_->Log("Joining EMImageProcessorPool Thread " + std::to_string(i), 0);
        EncoderThreads_[i].join();
    }

}


// Queue Access Functions
void ImageProcessorPool::EnqueueTask(ProcessingTask* _Task) {

    {
        std::lock_guard<std::mutex> LockQueue(QueueMutex_);
        Queue_.emplace(_Task);
        QueuedTasks_.fetch_add(1, std::memory_order_relaxed);
    }
    WorkAvailable_.notify_one();
}

int ImageProcessorPool::GetQueueSize() {
    return QueuedTasks_.load(std::memory_order_relaxed);
}

int ImageProcessorPool::GetOutstandingTaskCount() {
    return QueuedTasks_.load(std::memory_order_relaxed) + ActiveTasks_.load(std::memory_order_acquire);
}

bool ImageProcessorPool::DequeueTask(ProcessingTask** _Task) {

    // Firstly, Ensure Nobody Else Is Using The Queue
    std::lock_guard<std::mutex> LockQueue(QueueMutex_);

    // If the queue isn't empty, we grab the first element
    if (Queue_.size() > 0) {
        *_Task = Queue_.front();
        Queue_.pop();
        QueuedTasks_.fetch_sub(1, std::memory_order_relaxed);
        ActiveTasks_.fetch_add(1, std::memory_order_relaxed);

        return true;
    }

    return false;
}


// Public Enqueue Function
void ImageProcessorPool::QueueEncodeOperation(ProcessingTask* _Task) {
    EnqueueTask(_Task);
}

void ImageProcessorPool::WaitUntilIdle() {
    std::unique_lock<std::mutex> Lock(IdleMutex_);
    IdleCondition_.wait(Lock, [this] {
        return (QueuedTasks_.load(std::memory_order_relaxed) == 0) &&
               (ActiveTasks_.load(std::memory_order_acquire) == 0);
    });
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
