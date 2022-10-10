########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_CLASS_VisualRenderer

            # Add Source Files (.cpp)
            "ERS_CLASS_VisualRenderer.cpp"
            "ERS_CLASS_IconRenderer.cpp"
            "ERS_CLASS_Grid.cpp"
            "ERS_CLASS_InputProcessor.cpp"
            "ERS_CLASS_MeshRenderer.cpp"
            "ERS_CLASS_DepthMaps.cpp"
            "ERS_CLASS_ShadowMaps.cpp"
            "ERS_CLASS_ViewportOverlay.cpp"
            "ERS_CLASS_ViewportMenu.cpp"
            "ERS_CLASS_BoundingBoxRenderer.cpp"
            "ERS_FUNCTION_FindShaderByName.cpp"
            "ERS_FUNCTION_CheckForMeshTransparency.cpp"
            "ERS_FUNCTION_MeshTransparencySort.cpp"
            "ERS_FUNCTION_DrawMesh.cpp"
            "ERS_FUNCTION_DrawMeshNoTextures.cpp"
            "ERS_FUNCTION_ResetMeshTexture.cpp"
            "ERS_FUNCTION_ConvertRotationToFrontVector.cpp"
            "ERS_FUNCTION_MeshTransparencySortNoTextures.cpp"
            "ERS_FUNCTION_SetShaderUniformData.cpp"

            

            # Add Header Files (.h)
            "ERS_CLASS_VisualRenderer.h"
            "ERS_CLASS_IconRenderer.h"
            "ERS_CLASS_Grid.h"
            "ERS_CLASS_InputProcessor.h"
            "ERS_CLASS_MeshRenderer.h"
            "ERS_CLASS_DepthMaps.h"
            "ERS_CLASS_ShadowMaps.h"
            "ERS_CLASS_ViewportOverlay.h"
            "ERS_CLASS_ViewportMenu.h"
            "ERS_CLASS_BoundingBoxRenderer.h"
            "ERS_FUNCTION_FindShaderByName.h"
            "ERS_FUNCTION_CheckForMeshTransparency.h"
            "ERS_FUNCTION_MeshTransparencySort.h"
            "ERS_FUNCTION_DrawMesh.h"
            "ERS_FUNCTION_DrawMeshNoTextures.h"
            "ERS_FUNCTION_ResetMeshTexture.h"
            "ERS_FUNCTION_ConvertRotationToFrontVector.h"
            "ERS_FUNCTION_MeshTransparencySortNoTextures.h"
            "ERS_FUNCTION_SetShaderUniformData.h"




            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_CLASS_VisualRenderer
    yaml-cpp
    glm
    glad
    glfw
    IMGUI
    )

# Link Internal Libs
target_link_libraries(ERS_CLASS_VisualRenderer
    ERS_STRUCT_Camera
    ERS_STRUCT_SceneCamera
    
    ERS_LoggingSystem

    ERS_SceneManager
    ERS_ShaderLoader
    ERS_SceneLoader

    ERS_Editor_3DCursor

    ERS_STRUCT_LocRotScale
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_Viewport
    ERS_STRUCT_Shader
    ERS_STRUCT_Model
    ERS_STRUCT_DepthMap
    ERS_STRUCT_Scene
    ERS_STRUCT_ProjectUtils
    ERS_STRUCT_OpenGLDefaults
    ERS_STRUCT_RendererSettings
    ERS_STRUCT_ShaderUniformData

    )

target_include_directories(ERS_CLASS_VisualRenderer PUBLIC ./)

