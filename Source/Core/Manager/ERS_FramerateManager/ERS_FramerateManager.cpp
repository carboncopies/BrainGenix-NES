//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_FramerateManager.h>


ERS_CLASS_FramerateManager::ERS_CLASS_FramerateManager() {

    SetTargetFramerate(60);

}

ERS_CLASS_FramerateManager::~ERS_CLASS_FramerateManager() {

}

void ERS_CLASS_FramerateManager::SetTargetFramerate(int Framerate) {

    TargetFrameRate_ = Framerate;

    // Calculate Target Frame Time
    TargetFrameTime_ = 1.0f/Framerate;

}

void ERS_CLASS_FramerateManager::StartNewFrame() {

    // Get Current Time, Set To Start Time
    FrameStartTime_ = glfwGetTime();

    // Check Sync To Monitor
    if (SyncToMonitor_ != LastFrameSyncToMonitor_) {

        // If Sync To Monitor Set To True
        if (SyncToMonitor_) {
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }
        LastFrameSyncToMonitor_ = SyncToMonitor_;
    }

}

void ERS_CLASS_FramerateManager::DelayUntilNextFrame() {

    // Get Current Time
    FrameEndTime_ = glfwGetTime();
    auto SystemFrameEndTime = std::chrono::system_clock::now();

    // Calculate Delta Time
    double FrameTime = (FrameEndTime_ - FrameStartTime_);
    LastFrameTime_ = FrameTime;

    FrameSamples_.push_back(FrameTime);
    if (FrameSamples_.size() > NumberSamples_) {
        FrameSamples_.erase(FrameSamples_.begin());
    }
    ActualFrameTimes_.push_back(FrameTime);
    if (ActualFrameTimes_.size() > 1000) {
        ActualFrameTimes_.erase(ActualFrameTimes_.begin());
    }
    ActualFrameTimesMS_.push_back((float)(FrameTime * 1000));
    if (ActualFrameTimesMS_.size() > 1000) {
        ActualFrameTimesMS_.erase(ActualFrameTimesMS_.begin());
    }

    // Calc Average
    double SumFrameTimes = 0.0f;
    for (int i = 0; i < FrameSamples_.size(); i++) {
        SumFrameTimes += FrameSamples_[i];
    }
    AverageFrameTime_ = SumFrameTimes / FrameSamples_.size();

    // Add To Avg Array
    AverageFrameTimes_.push_back(AverageFrameTime_);
    if (AverageFrameTimes_.size() > 1000) {
        AverageFrameTimes_.erase(AverageFrameTimes_.begin());
    }

    // Sleep For Duration
    if ((!UnlockFramerate_) && (!SyncToMonitor_)) {
        std::this_thread::sleep_until(SystemFrameEndTime + std::chrono::nanoseconds((int)((TargetFrameTime_ - AverageFrameTime_)*1000000000)));
    }

}

float ERS_CLASS_FramerateManager::GetLastFrameTime() {

    // Return Exact Last Frame Time
    return LastFrameTime_;

}