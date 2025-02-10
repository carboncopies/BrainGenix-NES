//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <chrono>
#include <filesystem>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <math.h>


// Third-Party Libraries (BG convention: use <> instead of "")
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

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


// Thread Main Function
void ImageProcessorPool::EncoderThreadMainFunction(int _ThreadNumber) {

    // Set thread Name
    pthread_setname_np(pthread_self(), std::string("EM Image Processor Pool Thread " + std::to_string(_ThreadNumber)).c_str());

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
                    VoxelType PresentingVoxel = Task->Array_->GetVoxel(XVoxelIndex, YVoxelIndex, Task->VoxelZ);
                    // uint8_t DarkestVoxel = __UINT8_MAX__;
                    // for (size_t ZIndex = Task->VoxelZ; ZIndex < Task->VoxelZ + Task->SliceThickness_vox; ZIndex++) {

                    //     // Get Voxel At Position
                    //     VoxelType ThisVoxel = Task->Array_->GetVoxel(XVoxelIndex, YVoxelIndex, ZIndex);
                    //     // if (ThisVoxel.State_ == VOXELSTATE_INTENSITY) {
                    //         // if (ThisVoxel.Intensity_ < DarkestVoxel) {
                    //     PresentingVoxel = ThisVoxel;
                    //     DarkestVoxel = ThisVoxel.Intensity_;
                    //         // }
                    //     // Else, we have a special debug voxel, set this to be the one shown, and exit.
                    //     // } else {
                    //     //     PresentingVoxel = ThisVoxel;
                    //     //     break;
                    //     // }

                    // }


                    // Calculate Pixel Index
                    int ThisPixelX = XVoxelIndex - Task->VoxelStartingX;
                    int ThisPixelY = YVoxelIndex - Task->VoxelStartingY;

                    // Calculate Color To Be Set
                    if (PresentingVoxel.State_ == VoxelState_BLACK) {
                        OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 0);
                        continue;
                    } else if (PresentingVoxel.State_ == VoxelState_WHITE) {
                        OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 255);
                        continue;
                    } else if (PresentingVoxel.State_ == VoxelState_EMPTY) {
                        OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, 240); // <-- THAT IS THE DEFAULT IMAGE COLOR, SHOULD BE CONFIGURABLE
                        continue;                    
                    }

                    // If we've gotten this far, the voxel must be inside something
                    // then we set the color based on the perlin noise, and distance to edge
                    uint8_t Intensity;
                    if (Task->Params_->GeneratePerlinNoise_) {
                        float X = Task->Array_->GetXPositionAtIndex(XVoxelIndex);
                        float Y = Task->Array_->GetYPositionAtIndex(YVoxelIndex);
                        float Z = Task->Array_->GetZPositionAtIndex(Task->VoxelZ);
                        Intensity = GenerateVoxelColor(X, Y, Z, Task->Params_, Task->Generator_);
                    } else {
                        Intensity = Task->Params_->DefaultIntensity_;
                    }

                    if (Task->Params_->RenderBorders) {
                        Intensity = CalculateBorderColor(Intensity, PresentingVoxel.DistanceToEdge_vox_, Task->Params_);
                    }

                    OneToOneVoxelImage.SetPixel(ThisPixelX, ThisPixelY, Intensity);
                        

                }
            }

            // Note, when we do image processing (for like noise and that stuff, we should do it here!) (or after resizing depending on what is needed)
            // so then this will be phase two, and phase 3 is saving after processing




            // Contrast/Brightness Adjustments
            if (Task->AdjustContrast) {
                
                // Calculate the contrast/brightness for this image
                float ThisContrast = Task->Contrast + ((-1+2*((float)rand())/RAND_MAX) * Task->ContrastRandomAmount);
                float ThisBrightness = Task->Brightness + ((-1+2*((float)rand())/RAND_MAX) * Task->BrightnessRandomAmount);

                for (size_t X = 0; X < OneToOneVoxelImage.Width_px; X++) {
                    for (size_t Y = 0; Y < OneToOneVoxelImage.Height_px; Y++) {

                        int Color = OneToOneVoxelImage.GetPixel(X, Y);
                        Color = (ThisContrast * ((float)Color - 128.)) + 128 + ThisBrightness;
                        Color = std::clamp(Color, 0, 255);
                        OneToOneVoxelImage.SetPixel(X, Y, Color);

                    }
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
                    for (size_t Y = 0; Y < OneToOneVoxelImage.Height_px; Y++) {

                        int Color = OneToOneVoxelImage.GetPixel(X, Y);
                        float PositionX = (Task->VoxelStartingX + X) * Task->VoxelScale_um;
                        float PositionY = (Task->VoxelStartingY + Y) * Task->VoxelScale_um;
                        float ScaledPositionX = (PositionX + ZOffset) + (sin(PositionY * Task->InterferencePatternWobbleFrequency) * Task->InterferencePatternYAxisWobbleIntensity); 
                        Color += sin(Task->InterferencePatternXScale_um * ScaledPositionX) * ThisImageAmplitude + Task->InterferencePatternBias;
                        Color = std::clamp(Color, 0, 255);
                        OneToOneVoxelImage.SetPixel(X, Y, Color);

                    }
                }
            }


            // Pre-Blur Noise Pass
            if (Task->EnableImageNoise) {
                for (unsigned int i = 0; i < Task->PreBlurNoisePasses; i++) {
                    for (size_t X = 0; X < OneToOneVoxelImage.Width_px; X++) {
                        for (size_t Y = 0; Y < OneToOneVoxelImage.Height_px; Y++) {

                            int Color = OneToOneVoxelImage.GetPixel(X, Y);
                            Color += (RandomGenerator() % Task->ImageNoiseAmount) - int(Task->ImageNoiseAmount/2);
                            Color = std::clamp(Color, 0, 255);
                            OneToOneVoxelImage.SetPixel(X, Y, Color);

                        }
                    }
                }
            }

            // Perform Gaussian Blurring Step
            if (Task->EnableGaussianBlur) {
                iir_gauss_blur(OneToOneVoxelImage.Width_px, OneToOneVoxelImage.Height_px, 1, OneToOneVoxelImage.Data_.get(), Task->GaussianBlurSigma);
            }


            // Post-Blur Noise Pass
            if (Task->EnableImageNoise) {
                for (unsigned int i = 0; i < Task->PostBlurNoisePasses; i++) {
                    for (size_t X = 0; X < OneToOneVoxelImage.Width_px; X++) {
                        for (size_t Y = 0; Y < OneToOneVoxelImage.Height_px; Y++) {

                            int Color = OneToOneVoxelImage.GetPixel(X, Y);
                            Color += (RandomGenerator() % Task->ImageNoiseAmount) - int(Task->ImageNoiseAmount/2);
                            Color = std::clamp(Color, 0, 255);
                            OneToOneVoxelImage.SetPixel(X, Y, Color);

                        }
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