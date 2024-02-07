//=================================//
// This file is part of BrainGenix //
//=================================//

/*
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
#include <mutex>
#include <chrono>
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <vsg/all.h>


// #include <vsgXchange/all.h>
// #include <OpenImageIO/imageio.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/State/RenderData.h>
#include <BG/Renderer/State/Scene.h>

#include <BG/Renderer/SceneGraph/OffscreenRenderHelpers.h>
#include <BG/Renderer/SceneGraph/CameraHelpers.h>

#include <BG/Renderer/EncoderPool/Image.h>


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

    std::mutex                         SceneAccessLock_; /**Mutex to control access to the scene, so only one thread may use it at a time*/

    bool Initialized_ = false;                       /**Boolean that indicates if the renderer has been fully setup yet or not*/

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
    bool Initialize(bool _Windowed = false);


    /**
     * @brief Initialization function that iniitalizes the Scene struct in the renderer.
     * Will not build a scene with anything in it, just sets up a dumb empty scene.
     * 
     * @return true 
     * @return false 
     */
    bool SetupScene();


    /**
     * @brief Call this after changing the scene - updates the command graph and other attributes so it's ready to draw.
     * Will return true on success, false otherwise.
     * 
     * @return true 
     * @return false 
     */
    bool CompileScene();


    /**
     * @brief Sets up the viewport and other stuff associated with preparing to render.
     * Helps to initialize the camera and other associated things.
     * 
     * @return true 
     * @return false 
     */
    bool SetupViewer();


    /**
     * @brief Creates and initializes the camera struct.
     * 
     * @return true 
     * @return false 
     */
    bool SetupCamera();


    /**
     * @brief Runs only when in windowed mode, creates a window.
     * When running headless, this is not needed.
     * 
     * @return true 
     * @return false 
     */
    bool Windowed_CreateWindow();

    /**
     * @brief Runs only when in windowed mode, creates the event handler for the trackball.
     * Lets the user interact with the window and rotate stuff (in windowed mode).
     * 
     * @return true 
     * @return false 
     */
    bool Windowed_SetupEventHandler();


    /**
     * @brief Creates the command graph for windowed mode.
     * 
     * @return true 
     * @return false 
     */
    bool Windowed_SetupCommandGraph();


    /**
     * @brief Creates the command graph on headless mode.
     * 
     * @return true 
     * @return false 
     */
    bool Headless_SetupCommandGraph();


    /**
     * @brief Only runs in headless mode, sets up the vulkan device stuff.
     * 
     * @return true 
     * @return false 
     */
    bool Headless_SetupDevice();

    
    /**
     * @brief Run for headless mode, sets up a bunch of buffers for us to render into. (i guess?)
     * 
     * @return true 
     * @return false 
     */
    bool Headless_CreateRenderingBuffers();


    /**
     * @brief Updates the rendering buffers between frames. Only used in headless mode.
     * 
     * @return true 
     * @return false 
     */
    bool Headless_UpdateRenderingBuffers();

    /**
     * @brief Gets the image that was rendered last frame.
     * 
     * @param _Image Place to write the image we just captured.
     * 
     * @return true 
     * @return false 
     */
    bool Headless_GetImage(Image* _Image);


    /**
     * @brief Erases all objects in the current scene. Resets the scene to an empty state.
     * 
     * @return true 
     * @return false 
     */
    bool ClearScene();


    /**
     * @brief Helper that sets the headless resolution for renders.
     * Only has an effect when running headless, and will cause the next frame rendered to have the desired resolution.
     * 
     * @param _Width Desired width in pixels.
     * @param _Height Desired height in pixels.
     * @return true Success
     * @return false Failure
     */
    bool SetResolution(int _Width, int _Height);



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
     * 
     * @return true 
     * @return false 
     */
    bool DrawFrame();


    /**
     * @brief Render into the given image struct.
     * 
     * @param _Image 
     * @return true 
     * @return false 
     */
    bool RenderImage(Image* _Image);


    /**
     * @brief Moves the camera to the given position.
     * 
     * @param _Position
     * @param _LookAtPosition
     * @return true 
     * @return false 
     */
    bool UpdateCameraPosition(vsg::dvec3 _Position, vsg::dvec3 _LookAtPosition);

    /**
     * @brief Set the Camera's field of view
     * 
     * @param _FOV Field of view in degrees
     * 
     * @return true 
     * @return false 
     */
    bool SetCameraFOV(double _FOV_deg);


    /**
     * @brief Waits until the GPU is done doing stuff.
     * 
     */
    void WaitUntilGPUDone();




    // Thread Access/Control Functions

    /**
     * @brief Locks the scene mutex, allowing other threads to access the scene.
     * 
     */
    void LockScene();

    /**
     * @brief Unlocks the scene mutex, allowing other threads to access the scene.
     * 
     */
    void UnlockScene();




};





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG