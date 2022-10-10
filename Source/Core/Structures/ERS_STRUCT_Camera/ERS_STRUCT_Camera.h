//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>





// FOV zooming
enum class ZoomState{
	_IN, _OUT
};

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};


// Setup Camera Options
struct ERS_STRUCT_Camera {

public:



    /**
     * @brief Create the camera object.
     * 
     */
    ERS_STRUCT_Camera();

    /**
     * @brief Destroy the camera object.
     * 
     */
    ~ERS_STRUCT_Camera();


    
    /**
     * @brief Updates the camera's matricies, should be called once per frame.
     * 
     */
    void Update();



    /**
     * @brief Helper function, gets the projection/perspective matrix and the view matrix.
     * Note that if called before the update function, the values returned will be from the last frame.
     * 
     * @param Perspective 
     * @param View 
     */
    void GetMatrices(glm::mat4& Perspective, glm::mat4& View);

    /**
     * @brief Helper function, sets the camera's clipping distance.
     * 
     * @param NearClip Minimum distance in units for things to be rendered in.
     * @param FarClip Maximum distance in units for things to be rendered in.
     */
    void SetClipBoundries(float NearClip, float FarClip);

    /**
     * @brief Helper Function, gets the current camera clip distance.
     * 
     * @param NearClip Minimum distance in units for things to be rendered in.
     * @param FarClip Maximum distance in units for things to be rendered in.
     */
    void GetClipBoundires(float &NearClip, float &FarClip);

    /**
     * @brief Gets the current field of view angle.
     * 
     * @param FOV Field of view in degrees.
     */
    void GetFOV(float &FOV);
    float GetFOV();

    /**
     * @brief Sets the field of view angle.
     * 
     * @param FOV Field of view in degrees.
     */
    void SetFOV(float FOV);

    /**
     * @brief Set the camera's aspect ratio (width/height)
     * 
     * @param AspectRatio Aspect ratio (width/height)
     */
    void SetAspectRatio(float AspectRatio);

    /**
     * @brief Sets the rotation of the camera.
     * 
     * @param Rotation Rotation in degrees (x,y,z) or (pitch, yaw, roll)
     */
    void SetRotation(glm::vec3 Rotation);

    /**
     * @brief Get the rotation 
     * 
     * @param Rotation Rotation in degrees (x,y,z) or (pitch, yaw, roll)
     */
    void GetRotation(glm::vec3 &Rotation);
    glm::vec3 GetRotation();

    /**
     * @brief Set the position of the camera.
     * 
     * @param Position Position (x,y,z) in units relative to world origin.
     */
    void SetPosition(glm::vec3 Position);
    
    /**
     * @brief Get the position of the camera.
     * 
     * @param Position Position (x,y,z) in units relative to world origin.
     */
    void GetPosition(glm::vec3 &Position);
    glm::vec3 GetPosition();

    /**
     * @brief Get the camera's streaming priority.
     * 
     * @param StreamingPriority Int between 1-10 setting the priority. 
     */
    void GetStreamingPriority(int &StreamingPriority);
    int GetStreamingPriority();

    /**
     * @brief Set the Streaming Priority int.
     * 
     * @param StreamingPriority Int between 1-10 setting the priority. 
     */
    void SetStreamingPriority(int StreamingPriority);

private:

    // Camea Configuration Information
    float NearClip_          = 0.01f;  /**<Closest distance before geometry is culled.*/
    float FarClip_           = 100.0f; /**<Farthest distance before geometry is called*/
    float MinMovementSpeed_  = 0.01f;  /**<Slowest movement speed allowed in units per second*/
    float MaxMovementSpeed_  = 50.0f;  /**<Fastest movement speed allowed in units per second*/


    // Internal Camera State Information
    float MovementSpeed_     = 0.2f;   /**<Current Movement Speed*/
    float MouseSensitivity_  = 0.05f;  /**<Mouse sensitivity multiplier*/
    float FOV_               = 70.0f;  /**<Field of view in degrees*/
    float AspectRatio_       = 1.25f;  /**<Internal variable used to setup the projection matrix*/

    glm::vec3 Orientation_;            /**<Rotation of the camera in degrees. */
    glm::vec3 Position_;               /**<Position of the camera in units */
    
    glm::vec3 Front_;                  /**<Unit vector facing out the front of the camera. (in local space)*/
    glm::vec3 Up_;                     /**<Unit vector facing out the top of the camera. (in local space)*/
    glm::vec3 Right_;                  /**<Unit vector facing to the right of the camera. (in local space)*/

    glm::mat4 PerspectiveMatrix_;      /**<Perspective/projection matrix, updated every time the Update function is called*/
    glm::mat4 ViewMatrix_;             /**<View matrix, updated every time the Update function is called*/


    // Asset Streaming Settings
    int StreamingPriority_ = 1;        /**< Higher this is, the more the system will try and load assets for this camera. Should be in range (1-10)*/


};

