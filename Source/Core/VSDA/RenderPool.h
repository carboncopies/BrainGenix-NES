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

#include <VSDA/EM/EMRenderer.h>
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h>
#include <VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.h>

#include <VSDA/Ca/CaRenderer.h>
#include <VSDA/Ca/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h>
#include <VSDA/Ca/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.h>

#include <BG/Renderer/Interface.h>




namespace BG {
namespace NES {
namespace Simulator {
namespace VSDA {






/**
 * @brief This class creates a threadpool which owns all renderer instances.
 * This allows (on supported systems) for multiple renderers to exist at the same time, and as such render multiple simulations simultaineously.
 * 
 */
class RenderPool {


private:

    BG::Common::Logger::LoggingSystem*                        Logger_ = nullptr;   /**Pointer to instance of logging system*/

    std::unique_ptr<ImageProcessorPool>                       EMImageProcessorPool_; /**Instance of the ImageProcessorPool, which saves all required images to disk*/
    std::unique_ptr<VoxelArrayGenerator::ArrayGeneratorPool>  EMArrayGeneratorPool_; /**Instance of the ArrayGeneratorPool, used to parallelize rasterizing shapes into the voxel array with many threads*/

    std::unique_ptr<::BG::NES::VSDA::Calcium::ImageProcessorPool>                       CalciumImageProcessorPool_; /**Instance of the ImageProcessorPool, which saves all required images to disk*/
    std::unique_ptr<::BG::NES::VSDA::Calcium::VoxelArrayGenerator::ArrayGeneratorPool>  CalciumArrayGeneratorPool_; /**Instance of the ArrayGeneratorPool, used to parallelize rasterizing shapes into the voxel array with many threads*/

    bool                                                      Windowed_ = false;   /**Boolean indicating if we're making windowed or headless renderers*/

    std::mutex                                                QueueMutex_;         /**Mutex used to lock access to the queue when it's being modified*/
    std::queue<Simulation*>                                   Queue_;              /**Queue that contains simulations that need to be rendered*/

    std::vector<std::thread>                                  RenderThreads_;      /**List of rendering threads - each one tries to dequeue stuff from the queue to work on.*/
    std::atomic_bool                                          ThreadControlFlag_;  /**Bool that signals threads to exit*/



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
     * @brief Thread safe get simulation* from queue function. 
     * Will return false if there is nothing to be dequeued.
     * Otherwise will update the ptr given as a parameter.
     * 
     * @return true
     * @return false
     */
    bool DequeueSimulation(Simulation** _SimPtr);



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
     * @param _Windowed
     * @param _NumThreads 
     */
    RenderPool(BG::Common::Logger::LoggingSystem* _Logger, bool _Windowed = false, int _NumThreads = 1);

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