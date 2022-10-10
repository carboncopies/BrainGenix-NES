//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <string>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>

#include <ERS_SceneManager.h>

#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_Viewport.h>



/**
 * @brief This class renders any ui/overlay info onto the viewport as requested by the viewport struct.
 * 
 */
class ERS_CLASS_ViewportOverlay {

private:

    ERS_STRUCT_SystemUtils*  SystemUtils_;  /**<Pointer to systemtuils resource*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_; /**<Pointer to project utils instance*/


public:

    /**
     * @brief Construct a new ers class viewportoverlay object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_ViewportOverlay(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils);

    /**
     * @brief Destroy the ers class viewportoverlay object
     * 
     */
    ~ERS_CLASS_ViewportOverlay();

    /**
     * @brief Draw the overlay on the viewport pointer passed in.
     * 
     * @param Viewport 
     */
    void DrawOverlay(ERS_STRUCT_Viewport* Viewport);

};