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
#include <ERS_CLASS_ShadowMaps.h>

#include <ERS_SceneManager.h>

#include <ERS_STRUCT_ProjectUtils.h>
#include <ERS_STRUCT_SystemUtils.h>
#include <ERS_STRUCT_Viewport.h>
#include <ERS_STRUCT_Scene.h>



/**
 * @brief This class renders any ui/Menu info onto the viewport as requested by the viewport struct.
 * 
 */
class ERS_CLASS_ViewportMenu {

private:

    ERS_STRUCT_SystemUtils*  SystemUtils_ =   nullptr; /**<Pointer to systemtuils resource*/
    ERS_STRUCT_ProjectUtils* ProjectUtils_ =  nullptr; /**<Pointer to project utils instance*/
    bool*                    IsEditorMode_ =  nullptr; /**<Pointer to bool setting editor mode to enabled or disablewd*/
    double*                  GameStartTime_ = nullptr; /**<Pointer to float setting the glfw time when the game clock started*/
    
    std::vector<std::unique_ptr<ERS_STRUCT_Shader>>* Shaders_ = nullptr; /**<Pointer to vector containing shader information*/

public:

    /**
     * @brief Construct a new ers class viewportMenu object
     * 
     * @param SystemUtils 
     */
    ERS_CLASS_ViewportMenu(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, double* GameStartTime, bool* IsEditorMode, std::vector<std::unique_ptr<ERS_STRUCT_Shader>>* Shaders);

    /**
     * @brief Destroy the ers class viewportMenu object
     * 
     */
    ~ERS_CLASS_ViewportMenu();

    /**
     * @brief Draw the Menu on the viewport pointer passed in.
     * 
     * @param Viewport 
     */
    void DrawMenu(ERS_STRUCT_Viewport* Viewport, ERS_CLASS_ShadowMaps* ShadowMaps);


    void AddPointLight(ERS_CLASS_ShadowMaps* ShadowMaps);
    void AddDirectionalLight(ERS_CLASS_ShadowMaps* ShadowMaps);
    void AddSpotLight(ERS_CLASS_ShadowMaps* ShadowMaps);
    void AddScene();
    void AddSceneCamera();
    

};