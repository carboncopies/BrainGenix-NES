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


// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/SceneGraph/Manager.h>

#include <BG/Renderer/SceneGraph/GeometryBuilder.h>
#include <BG/Renderer/SceneGraph/Primitive/Cube.h>
#include <BG/Renderer/EncoderPool/Image.h>



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

    std::thread WindowedUpdaterThread_; /**This thread only exists in windowed mode and simply renders frames constantly.*/
    bool ThreadExit_ = false;
    bool IsWindowed_ = false;

    void WindowedThread();

public:

    /**
     * @brief Constructor for the rendering system.
     * 
     * @param _Logger Pointer to instance of the bg logging system.
    */
    Interface(BG::Common::Logger::LoggingSystem* _Logger);

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
    */
    bool Initialize(bool _Windowed = false);

    /**
     * @brief Does what the name suggests - draws a frame on the window or offscreen if rendering headless.
     * 
     * 
     * @return true 
     * @return false 
     */
    bool DrawFrame();

    /**
     * @brief Render into the given image struct.
     * 
     * @param _Image Valid instance of struct to render image to.
     * @return true 
     * @return false 
     */
    bool RenderImage(Image* _Image);


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
     * @brief Recompile scene graph. Call this after modifying the scene's geometry.
     * Returns true on succes, false otherwise.
     * 
     * @return true 
     * @return false
     */
    bool UpdateScene();

    /**
     * @brief Waits until VSG is done doing things (if drawing a frame).
     * 
     */
    void WaitUntilGPUDone();

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

    /**
     * @brief Setup a default viewer position.
     * 
     */
    void ResetViewer();

    /**
     * @brief Clears the scene, resetting it to the default state.
     * 
     */
    void ResetScene();



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
     * @brief Update the camera FOV.
     * 
     * @param _FOV 
     * @return true 
     * @return false 
     */
    bool SetCameraFOV(double _FOV);

};





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG