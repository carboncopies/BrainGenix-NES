//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file defines the interface for the slice generator, which creates voxel representations of a given simulation.
    Additional Notes: None
    Date Created: 2023-11-29
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
#include <filesystem>

// Third-Party Libraries (BG convention: use <> instead of "")


// Internal Libraries (BG convention: use <> instead of "")
#include <Simulator/Structs/Simulation.h>

#include <VSDA/SliceGenerator/Structs/MicroscopeParameters.h>
#include <VSDA/SliceGenerator/Structs/VoxelArray.h>

#include <BG/Renderer/Interface.h>
#include <BG/Renderer/SceneGraph/Primitive/Cube.h>
#include <BG/Renderer/EncoderPool/Image.h>
#include <BG/Renderer/EncoderPool/EncoderPool.h>

#include <BG/Common/Logger/Logger.h>


namespace BG {
namespace NES {
namespace Simulator {



// Target Process: Simulation -> Voxels -> Meshes -> Images
// Firstly, we take a simulation pointer, build a voxel array from it
// Then, we slice the voxel array one slice at a time, and build a mesh from it
// Then the mesh is used to generate an image

/**
 * @brief Helper function for VSDA subsystem.
 * Will initialize the VSDA struct within the simulation struct - for further use later.
 * 
 * @param _Logger Pointer to logging system instance
 * @param _Sim Pointer to simulation struct that we can modify with this state
 * 
 * @return true On Success
 * @return false On Error
 */
bool VSDA_EM_Initialize(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim);

/**
 * @brief Setup the virtual microscope on a given simulation with the given parameters.
 * 
 * @param _Logger Pointer to logging system instance.
 * @param _Sim Pointer to simulation instance which we will modify with our new data.
 * @param _Params Microscope parameter struct used to update the VSDAData_ struct in the sim.
 * 
 * @return true On Success
 * @return false On Error
 */
bool VSDA_EM_SetupMicroscope(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters _Params);

/**
 * @brief Defines the region that we'd like to scan with the current render operation.
 * Currently, we only support rectangular regions defined by two points in space. That's it for now unfortunately.
 * 
 * @param _Logger Pointer to logging system interface.
 * @param _Sim Pointer to simulation instance to be configured by this VSDA operation.
 * @param _ScanRegion Instance of the BoundingBox struct with the needed values populated.
 * @param _RegionID Pointer to int, which will contain the scan region id returned, if any. Check bool status to indicate if this will be set or not.
 * 
 * @return true On Success
 * @return false On Error
 */
bool VSDA_EM_DefineScanRegion(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, ScanRegion _ScanRegion, int* _RegionID);

/**
 * @brief Sets the enum flag in the simulation that a render operation has been requested.
 * Probably should also check to ensure that all needed data has been provided before queuing this operation but we can add that later.
 * 
 * @param _Logger Pointer to logging system interface.
 * @param _Sim Pointer to simulation instance to be configured by this instance.
 * @param _RegionID Valid index of region to be rendered in this call.
 * 
 * @return true On Success
 * @return false On Error
 */
bool VSDA_EM_QueueRenderOperation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, int _RegionID);



/**
 * @brief Create a Voxel Array From Simulation object
 * Generates a voxel array at pointer _Array, within the region defined in _Region, from the simulation _Sim.
 * 
 * @param _Logger Pointer to logging system instance
 * @param _Sim Pointer to simulation that data is to be generated from
 * @param _Region Pointer to region in that simulation where we'll be generating an array
 * @param _Array Pointer to array to be populated.
 * @return true On success
 * @return false On failure (eg: out of memory, out of bounds, etc.)
 */
bool CreateVoxelArrayFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters* _Params, VoxelArray* _Array);

/**
 * @brief Render the given slice from an array to the renderer's screen
 * 
 * @param _Logger 
 * @param _VSDAData 
 * @param _FileNameArray
 * @param _FilePrefix
 * @param _SliceNumber 
 * @return true 
 * @return false 
 */
bool RenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, VSDAData* _VSDAData, std::vector<std::string>* _FileNameArray, std::string _FilePrefix, int SliceNumber, BG::NES::Renderer::EncoderPool* _EncoderPool);

/**
 * @brief Render the given slice from an array to the renderer's screen. Uses vulkan to do so.
 * 
 * @param _Logger 
 * @param _Renderer
 * @param _VSDAData 
 * @param _FileNameArray
 * @param _FilePrefix
 * @param _SliceNumber 
 * @return true 
 * @return false 
 */
bool VulkanRenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, Renderer::Interface* _Renderer, VSDAData* _VSDAData, std::vector<std::string>* _FileNameArray, std::string _FilePrefix, int SliceNumber, BG::NES::Renderer::EncoderPool* _EncoderPool);



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
