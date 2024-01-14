//=================================//
// This file is part of BrainGenix //
//=================================//

/*
    Date Created: 2023-10-30
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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vsg/all.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

#include <BG/Renderer/SceneGraph/Primitive/Cube.h>
#include <BG/Renderer/SceneGraph/Primitive/Sphere.h>
#include <BG/Renderer/SceneGraph/Primitive/Cylinder.h>
#include <BG/Renderer/SceneGraph/Shader/Shader.h>
#include <BG/Renderer/SceneGraph/Shader/Phong.h>
#include <BG/Renderer/State/RenderData.h>
#include <BG/Renderer/State/Scene.h>
#include <BG/Renderer/Math/MathHelpers.h>


namespace BG {
namespace NES {
namespace Renderer {



/**
 * @brief This class provides an interface for creating geometric primitives in the scene (e.g cubes, spheres, cylinders, etc.)
 * The instance of this class can be accessed via the interface which will allow for other parts of the system to create shapes in the scene.
 * This will likely be used to create a 3d representation of the simulation.
 */
class GeometryBuilder {

private: 

    // Logging Stuff
    BG::Common::Logger::LoggingSystem *Logger_ = nullptr; /**Logging Class Pointer*/


    // VSG Builder Utils
    vsg::ref_ptr<vsg::Builder> Builder_; /**Instance of the vsg builder class. Used to create 3d primitives.*/


    /**
     * @brief Sets up the builder with the right shader according to the struct pointer passed in.
     * 
     * @param _Shader Pointer to shader struct instance.
     * @return true 
     * @return false 
     */
    bool ShaderHandler(Shaders::Shader* _Shader);


public:

    /**
     * @brief Constructor for the geometry builder.
    */
    GeometryBuilder(BG::Common::Logger::LoggingSystem* _Logger);

    /**
     * @brief Destructor to the renderer, pretty self-explanitory.
    */
    ~GeometryBuilder();


    /**
     * @brief Adds a cube to the given scene, with the information defined in the Cube struct.
     * Returns true on success, false otherwise.
     * 
     * @param _Scene VSG Group instance wrapped by vsg::ref_ptr
     * @param _CubeCreateInfo Pointer to cube create information.
     * @return true 
     * @return false 
     */
    bool CreateCube(vsg::ref_ptr<vsg::Group> _Scene, Primitive::Cube* _CubeCreateInfo);

    /**
     * @brief Adds a sphere to the given scene, with the information defined in the Sphere struct.
     * Returns true on success, false otherwise.
     * 
     * @param _Scene VSG Group instance wrapped by vsg::ref_ptr
     * @param _SphereCreateInfo Pointer to Sphere create information.
     * @return true 
     * @return false 
     */
    bool CreateSphere(vsg::ref_ptr<vsg::Group> _Scene, Primitive::Sphere* _SphereCreateInfo);
      
    /**
     * @brief Adds a cylinder to the given scene, with the information defined in the Cylinder struct.
     * Returns true on success, false otherwise.
     * 
     * @param _Scene VSG Group instance wrapped by vsg::ref_ptr
     * @param _CylinderCreateInfo Pointer to Cylinder create information.
     * @return true 
     * @return false 
     */
    bool CreateCylinder(vsg::ref_ptr<vsg::Group> _Scene, Primitive::Cylinder* _CylinderCreateInfo);
      

};





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG