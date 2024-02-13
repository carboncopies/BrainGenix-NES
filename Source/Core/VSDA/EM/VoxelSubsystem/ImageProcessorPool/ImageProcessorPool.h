//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file creates a multithreaded image compression helper.
    Additional Notes: None
    Date Created: 2024-01-13
    Author(s): Thomas Liao


    Copyright (C) 2024  Thomas Liao

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once



// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <assert.h>
#include <memory>
#include <queue>
#include <thread>


// Third-Party Libraries (BG convention: use <> instead of "")



// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/Image.h>
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/ProcessingTask.h>




namespace BG {
namespace NES {
namespace Simulator {






/**
 * @brief This class creates a threadpool which compresses and saves images.
 * 
 */
class ImageProcessorPool {


private:

    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to instance of logging system*/

    std::mutex QueueMutex_;                               /**Mutex used to lock access to the queue when it's being modified*/
    std::queue<ProcessingTask*> Queue_;                            /**Queue that contains tasks to be compressed*/

    std::vector<std::thread> EncoderThreads_;             /**List of encoding threads - each one tries to dequeue stuff from the queue to work on.*/
    std::atomic_bool ThreadControlFlag_;                  /**Bool that signals threads to exit*/



    /**
     * @brief Thread safe enqueue function.
     * 
     * @param _Task 
     */
    void EnqueueTask(ProcessingTask* _Task);


    /**
     * @brief Thread safe get ProcessingTask* from queue function. 
     * Will return false if there is nothing to be dequeued.
     * Otherwise will update the ptr given as a parameter.
     * 
     * @return true
     * @return false
     */
    bool DequeueTask(ProcessingTask** _TaskPtr);



    /**
     * @brief Entry point for renderer threads.
     * 
     * @brief _ThreadNumber ID of this thread
     */
    void EncoderThreadMainFunction(int _ThreadNumber);


public:

    /**
     * @brief Initializes the imageprocessorpool with the given number of threads requested.
     * 
     * @param _Logger 
     * @param _NumThreads 
     */
    ImageProcessorPool(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads = 24);

    /**
     * @brief Destroys the render pool object.
     * 
     */
    ~ImageProcessorPool();


    /**
     * @brief Places the ProcessingTask struct into the queue to be picked up by an available worker thread.
     * 
     * @param _ProcessingTask 
     */
    void QueueEncodeOperation(ProcessingTask* _Task);

    /**
     * @brief Thread safe getSize function.
     * 
     * @return int 
     */
    int GetQueueSize();


};



}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG