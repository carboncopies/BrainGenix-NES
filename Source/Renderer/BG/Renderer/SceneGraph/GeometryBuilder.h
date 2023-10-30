//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Description: This file contains the NES Rendering Manager. It is responsible for the underlying initialization and ownership of the Vulkan subsystem.
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


// Third-Party Libraries (BG convention: use <> instead of "")
#include <vsg/all.h>
#include <vsgXchange/all.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/State/RenderData.h>
#include <BG/Renderer/State/Scene.h>


namespace BG {
namespace NES {
namespace Renderer {



/**
 * @brief The renderer manager is responsible for underlying Vulkan management (init, deinit, controling access, etc.)
 */
class Manager {

private: 

    BG::Common::Logger::LoggingSystem *Logger_ = nullptr; /**Logging Class Pointer*/

    std::unique_ptr<State::RenderData> RenderData_; /**Instance of renderdata struct, stores vulkan scene graph state*/
    std::unique_ptr<State::Scene>      Scene_;      /**Instance of scene struct, stores the geometry for this scene*/

    // todo:
    // migrate vsg argparser to function here
    // implement state structures for the rest of the code
    // add access functions to allow us to read the sim and create a rendered version of it
    // then work on shader functionality to recreate the TEM look
    // also work on making something so we can slice the shapes and create a cross section of it
    // (maybe geom shader?)


public:

    /**
     * @brief Constructor for the rendering system.
    */
    Manager(BG::Common::Logger::LoggingSystem* _Logger);

    /**
     * @brief Destructor to the renderer, pretty self-explanitory.
    */
    ~Manager();


    /**
     * @brief Initializes the Rendering interface. 
     * Use the _IsWindowed param for debugging by shwoing an SDL window containing the to-be-rendered content.
     * Note that use of this parameter is for debugging only and requires having either an X-Server or Wayland installed.
     * 
     * _IsDebugging as the name implies, toggles on and off debug/validation layers.
     * These layers serve to provide debug information for the application developers.
     * Only disable these after performing *extensive* testing, as bugs may not be noticed otherwise.
    */
    bool Initialize(int _NumArgs, char** _ArgValues);


    /**
     * @brief Initialization function that iniitalizes the Scene struct in the renderer.
     * Will not build a scene with anything in it, just sets up a dumb empty scene.
     * 
     * @return true 
     * @return false 
     */
    bool SetupScene();

    /**
     * @brief Sets up the viewport and other stuff associated with preparing to render.
     * Helps to initialize the camera and other associated things.
     * 
     * @return true 
     * @return false 
     */
    bool SetupViewer();



    /**
     * @brief Returns scene pointer.
     * Assumes that the scene has been initialized at this point.
     * 
     * @return vsg::ref_ptr<vsg::Group> 
     */
    vsg::ref_ptr<vsg::Group> GetScene();





    /**
     * @brief Does what it sounds like, this draws a frame on the renderer.
     * Returns true on success, false on failure.
     * 
     * @return true 
     * @return false 
     */
    bool DrawFrame();


};





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG