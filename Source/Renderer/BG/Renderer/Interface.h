//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains the NES Rendering system interface code.
    Additional Notes: None
    Date Created: 2023-09-17
    Author(s): Thomas Liao


    Copyright (C) 2023  Thomas Liao

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


// Third-Party Libraries (BG convention: use <> instead of "")
// #include <SDL.h>
// #include <SDL_vulkan.h>

#include <vsg/all.h>
#include <vsgXchange/all.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/SceneGraph/Manager.h>

#include <BG/Renderer/SceneGraph/GeometryBuilder.h>
#include <BG/Renderer/SceneGraph/Primitive/Cube.h>



namespace BG {
namespace NES {
namespace Renderer {



/**
 * @brief Rendering system interface for NES. This system is used to generate virtual scan data from models.
 */
class Interface {

private: 

    BG::Common::Logger::LoggingSystem *Logger_ = nullptr; /**Logging Class Pointer*/

    std::unique_ptr<Manager>         RendererManager_ = nullptr; /**Instance of the renderer manager, setup when the init call is made.*/
    std::unique_ptr<GeometryBuilder> GeometryBuilder_ = nullptr; /**Instance of the geometry builder, used to create various primitives in the scene*/

public:

    /**
     * @brief Constructor for the rendering system.
     * 
     * @param _Logger Pointer to instance of the bg logging system.
    */
    Interface(BG::Common::Logger::LoggingSystem* _Logge);

    /**
     * @brief Destructor to the renderer, pretty self-explanitory.
    */
    ~Interface();


    /**
     * @brief Initializes the Rendering interface. 
     * Use the EnableWindow param for debugging by shwoing an SDL window containing the to-be-rendered content.
     * Note that use of this parameter is for debugging only and requires having either an X-Server or Wayland installed.
     * 
     * EnableValidationLayers as the name implies, toggles on and off debug/validation layers.
     * These layers serve to provide debug information for the application developers.
     * Only disable these after performing *extensive* testing, as bugs may not be noticed otherwise.
     * 
     * @param _NumArgs Number of args gotten from the main func, known commonly as argc.
     * @param _ArgValues Values of each of the arguments. Known from main commonly as argv.
    */
    bool Initialize(int _NumArgs, char** _ArgValues);

    /**
     * @brief Does what the name suggests - draws a frame on the window or offscreen if rendering headless.
     * 
     * @return true 
     * @return false 
     */
    bool DrawFrame();


    /**
     * @brief Creates a box with the given parameters to the current scene graph.
     * Returns true on success, false otherwise.
     * 
     * @param _CreateInfo Instance of the cube create info struct - fully populated with the right info.
     * @return true 
     * @return false 
     */
    bool AddBox(Primitive::Cube* _CreateInfo);

    /**
     * @brief Creates a sphere with the given parameters to the current scene graph.
     * Returns true on success, false otherwise.
     * 
     * @param _CreateInfo Instance of the sphere create info struct - fully populated with the right info.
     * @return true 
     * @return false 
     */
    bool AddSphere(Primitive::Sphere* _CreateInfo);

    /**
     * @brief Creates a cylinder with the given parameters to the current scene graph.
     * Returns true on success, false otherwise.
     * 
     * @param _CreateInfo Instance of the cylinder create info struct - fully populated with the right info.
     * @return true 
     * @return false 
     */
    bool AddCylinder(Primitive::Cylinder* _CreateInfo);

    /**
     * @brief Recompile scene graph. Call this after modifying the scene's geometry.
     * Returns true on succes, false otherwise.
     * 
     * @return true 
     * @return false
     */
    bool UpdateScene();

};





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG