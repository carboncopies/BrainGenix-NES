//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>


// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <imgui.h>

#include <ImGuizmo.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>

#include <ERS_STRUCT_Camera.h>
#include <ERS_STRUCT_LocRotScale.h>



enum class GizmoMode
{
    none,
    translate,
    rotate,
    scale
};




/**
 * @brief Setup 3D Cursors for the user to edit with
 * 
 */
class Cursors3D {

private:

    float Matrix_[4][16] = { /**<Cube For Viewport Angle Identifier*/
        { 1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f },

        { 1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        2.f, 0.f, 0.f, 1.f },

        { 1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        2.f, 0.f, 2.f, 1.f },

        { 1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 2.f, 1.f }
    };


    bool   IsCursorActive_          = false;   /**<Indicates If Cursor Active Or Disabled*/
    bool   EnableRotation_          = false;   /**<Enable/Disable The Rotation Gizmo*/
    bool   EnableScale_             = false;   /**<Enable/Disable The Scale Gizmo*/ 
    bool   LastFrameActiveState_    = false;   /*<If the cursor was being used in the last frame or not*/ 
    float* CameraView_              = nullptr; /**<Camera View Matrix, Converted From GLM::Mat4*/
    float* CameraProjection_        = nullptr; /**<Camera Projection Matrix, Converted From GLM::Mat4*/
    float  GridSnapAmountTranslate_ = -1.0f;   /**<Unit To Snap To On Grid During Translation Mode*/
    float  GridSnapAmountRotate_    = -1.0f;   /**<Unit To Snap To On Grid During Rotation Mode*/
    float  GridSnapAmountScale_     = -1.0f;   /**<Unit To Snap To On Grid During Scale Mode*/
    int    DisableGizmoForFrames_   = 0;       /**<If nonzero, will disable the gizmo*/

    ERS_STRUCT_Camera* Camera_ = nullptr; /**<Camera Object Pointer*/
    ImGuizmo::OPERATION CurrentGizmoOperation_ = ImGuizmo::TRANSLATE; /**<Set Current Cursor Operation*/
    ImGuizmo::MODE GizmoMode_ = ImGuizmo::WORLD; /**<Set Default Coordinate Space*/

    glm::vec3 InitialPos_;   /**<Initial Scale, Used To Cancel Movement If Escape Is Pressed During An Operation*/
    glm::vec3 InitialRot_;   /**<Initial Scale, Used To Cancel Movement If Escape Is Pressed During An Operation*/
    glm::vec3 InitialScale_; /**<Initial Scale, Used To Cancel Movement If Escape Is Pressed During An Operation*/

public:

    glm::vec3 Pos_;
    glm::vec3 Rot_;
    glm::vec3 Scale_;

    bool HasObjectChanged_ = false; /*<Indicate If Update Needed*/

public:

    /**
     * @brief Construct a new Cursors 3D object
     * 
     */
    Cursors3D();

    /**
     * @brief Destroy the Cursors 3D object
     * 
     */
    ~Cursors3D();

    /**
     * @brief Returns if the gizmo is being used
     * 
     * @return true 
     * @return false 
     */
    bool IsUsing();

    /**
     * @brief Returns if the gizmo is being hovered by the mouse
     * 
     * @return true 
     * @return false 
     */
    bool IsHovered();


    /**
     * @brief Call this to tell the cursor that the object's position has changed and to update the cursor position.
     * 
     */
    void ObjectHasChanged();

    /**
     * @brief Draw the gizmo
     * 
     * @param Camera 
     * @param IsCameraMoving 
     */
    void Draw(ERS_STRUCT_Camera* Camera, bool IsCameraMoving, bool ShowCube, bool ShowCursor);

    /**
     * @brief Set the Loc Rot Scale object, accepts three glm::vec3 arrays indicating position, rotation and scale (in that order).
     * 
     * @param Pos 
     * @param Rot 
     * @param Scale 
     */
    void SetLocRotScale(glm::vec3 Pos, glm::vec3 Rot, glm::vec3 Scale, bool HasRotation, bool HasScale);


    /**
     * @brief Indicates if the cursor has changed from being not selected to selected or vise versa in the last frame.
     * 
     * @return true 
     * @return false 
     */
    bool HasStateChanged();


    /**
     * @brief Use this function to determine if grid snap is enabled and if so to what scale.
     * 
     * @param AmountTranslate 
     * @param AmountRotate 
     * @param AmountScale 
     */
    void SetGridSnap(float AmountTranslate, float AmountRotate, float AmountScale);


};