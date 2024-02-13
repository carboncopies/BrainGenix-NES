//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains the NES voxel array rasterization threadpool code.
    Additional Notes: None
    Date Created: 2024-01-20
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



#include <VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/Task.h>




namespace BG {
namespace NES {
namespace Simulator {
namespace VoxelArrayGenerator {






/**
 * @brief This class creates a threadpool which owns all renderer instances.
 * This allows (on supported systems) for multiple renderers to exist at the same time, and as such render multiple tasks simultaineously.
 * 
 */
class ArrayGeneratorPool {


private:

    BG::Common::Logger::LoggingSystem* Logger_ = nullptr;    /**Pointer to instance of logging system*/

    std::mutex QueueMutex_;                                  /**Mutex used to lock access to the queue when it's being modified*/
    std::queue<Task*> Queue_;                                /**Queue that contains tasks that need to be rendered*/

    std::vector<std::thread> RenderThreads_;                 /**List of rendering threads - each one tries to dequeue stuff from the queue to work on.*/
    std::atomic_bool ThreadControlFlag_;                     /**Bool that signals threads to exit*/



    /**
     * @brief Thread safe enqueue function.
     * 
     * @param _Sim 
     */
    void EnqueueTask(Task* _Sim);

    /**
     * @brief Thread safe getSize function.
     * 
     * @return int 
     */
    int GetQueueSize();

    /**
     * @brief Thread safe get task* from queue function. 
     * Will return false if there is nothing to be dequeued.
     * Otherwise will update the ptr given as a parameter.
     * 
     * @return true
     * @return false
     */
    bool DequeueTask(Task** _SimPtr);

    /**
     * @brief Decode many tasks at once, can help if threads are underutillized.
     * Does not guarentee the number of tasks returned will be equal to the number requested.
     * We only guarentee that it will not *exceed* the value specified.
     * 
     * For example, if the queue only has one thing in it, and three are requested, only one will be returned.
     * 
     * @param _NumTasks 
     * @return std::vector<Task*> 
     */
    std::vector<Task*> DequeueTasks(int _NumTasks=3);


    /**
     * @brief Entry point for renderer threads.
     * 
     * @brief _ThreadNumber ID of this thread
     */
    void RendererThreadMainFunction(int _ThreadNumber);


public:

    /**
     * @brief Initializes the arraygeneratorpool with the given number of threads requested.
     * 
     * @param _Logger 
     * @param _NumThreads 
     */
    ArrayGeneratorPool(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads = 1);

    /**
     * @brief Destroys the render pool object.
     * 
     */
    ~ArrayGeneratorPool();


    /**
     * @brief Places the task struct into the queue to be picked up by an available worker thread.
     * 
     * @param _Task 
     */
    void QueueWorkOperation(Task* _Task);

    /**
     * @brief Waits until the queue is empty.
     * NOTE: this does not guarentee everything is done, that would involve us having to ensure all threads completed the work too.
     * We're just waiting until the queue is empty, those threads could still be doing stuff. 
     * 
     */
    void BlockUntilQueueEmpty(bool _LogOutput = true);


};



}; // Close Namespace VoxelArrayGenerator
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG