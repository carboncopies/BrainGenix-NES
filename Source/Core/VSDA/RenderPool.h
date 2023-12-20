//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains the NES Rendering system interface code.
    Additional Notes: None
    Date Created: 2023-12-19
    Author(s): Thomas Liao, Randal Koene


    Copyright (C) 2023  Thomas Liao, Randal Koene

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

#include <Simulator/Structs/Simulation.h>

#include <BG/Renderer/Interface.h>




namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {






/**
 * @brief add doxygen later
 * 
 */
class RenderPool {


private:

    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to instance of logging system*/

    std::mutex QueueMutex_;                               /**Mutex used to lock access to the queue when it's being modified*/
    std::queue<Simulation*> Queue_;                       /**Queue that contains simulations that need to be rendered*/

    std::vector<std::thread> RenderThreads_;                            /** We will support multiple threads in the future but that doesnt work for now so you only get one :(*/


    /**
     * @brief Thread safe enqueue function.
     * 
     * @param _Sim 
     */
    void EnqueueSimulation(Simulation* _Sim);

    /**
     * @brief Thread safe getSize function.
     * 
     * @return int 
     */
    int GetQueueSize();

    /**
     * @brief Thread safe get simulation* from queue function. Will return null if queue is empty.
     * 
     * @return Simulation* 
     */
    Simulation* DequeueSimulation();



    /**
     * @brief Entry point for renderer threads.
     * 
     * @brief _ThreadNumber ID of this thread
     */
    void RendererThreadMainFunction(int _ThreadNumber);


public:

    /**
     * @brief Initializes the renderpool with the given number of threads requested.
     * Note that this may be subject to however many logical devices your vulkan hardware can support.
     * 
     * @param _Logger 
     * @param _NumThreads 
     */
    RenderPool(BG::Common::Logger::LoggingSystem* _Logger, int _NumThreads = 1);

    /**
     * @brief Destroys the render pool object.
     * 
     */
    ~RenderPool();


    /**
     * @brief Places the simulation struct into the queue to be picked up by an available renderer thread.
     * 
     * @param _Simulation 
     */
    void QueueRenderOperation(Simulation* _Simulation);


};



}; // Close Namespace VSDA
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG