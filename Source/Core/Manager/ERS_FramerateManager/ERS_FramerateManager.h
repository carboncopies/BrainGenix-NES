//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

#include <GLFW/glfw3.h>

// Internal Libraries (BG convention: use <> instead of "")



/**
 * @brief This class provides a system for managing the framerate. Additionally, it provides an interface for adjusting the target and identifying when targets are not met, etc.
 * 
 */
class ERS_CLASS_FramerateManager {

private:

    std::chrono::high_resolution_clock Clock_;        /**<High Res Clock Used For Frame Time Calcs*/
    std::vector<double>                FrameSamples_; /**<Sample Frame Times*/


    double TargetFrameTime_        = -1.0f; /**<Target Frame Time In MS*/
    double FrameStartTime_         = -1.0f; /**<Starting Time Of Frame*/
    double FrameEndTime_           = -1.0f; /**<Ending Time Of Frame*/
    double LastFrameTime_          = -1.0f; /**<Non-Averaged Last Frame Time*/
    float  AverageFrameTime_       = -1.0f; /**<Float representing the average time in ms*/
    int    NumberSamples_          = 20;    /**<Set number of samples used in average*/
    bool   SyncToMonitorChanged_   = true;  /**<Updates If Sync To Monitor Has Changed*/
    bool   LastFrameSyncToMonitor_ = false; /**<Check If Last Sync To Monitor Changed*/




public:

    int   TargetFrameRate_ = 60;    /**<Target Frame Rate In FPS*/
    bool  UnlockFramerate_ = false; /**<Run As Fast As Possible*/
    bool  SyncToMonitor_   = true;  /**<Sync To Monitor FPS*/


    std::vector<double> ActualFrameTimes_;   /**<List of last thousand actual measured frame times in seconds*/
    std::vector<double> AverageFrameTimes_;  /**<List of average frame times*/
    std::vector<float>  ActualFrameTimesMS_; /**<Frame Times In MS*/
    


public:

    /**
     * @brief Construct a new ers class frameratemanager object
     * 
     */
    ERS_CLASS_FramerateManager();

    /**
     * @brief Destroy the ers class frameratemanager object
     * 
     */
    ~ERS_CLASS_FramerateManager();
    


    /**
     * @brief Call at the start of every frame, starts the timer.
     * 
     */
    void StartNewFrame(); 

    /**
     * @brief Call at the end of every frame, delays to next frame if needed.
     * 
     */
    void DelayUntilNextFrame();

    /**
     * @brief Set the Target Framerate value.
     * 
     * @param TargetFramerate 
     */
    void SetTargetFramerate(int TargetFramerate);

    /**
     * @brief Get the Last Frame Time, and return as float
     * 
     * @return float 
     */
    float GetLastFrameTime();

};