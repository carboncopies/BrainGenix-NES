########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################

# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_SceneTree

            # Add Source Files (.cpp)
            "GUI_Window_SceneTree.cpp"
            "ERS_GUI_FUNCTION_DuplicateModel.cpp"
            "ERS_GUI_FUNCTION_DuplicateScene.cpp"
            "ERS_GUI_FUNCTION_DuplicatePointLight.cpp"
            "ERS_GUI_FUNCTION_DuplicateDirectionalLight.cpp"
            "ERS_GUI_FUNCTION_DuplicateSpotLight.cpp"
            "ERS_GUI_CLASS_PointLightRenameModal.cpp"
            "ERS_GUI_CLASS_DirectionalLightRenameModal.cpp"
            "ERS_GUI_CLASS_SpotLightRenameModal.cpp"
            "ERS_GUI_CLASS_SceneRenameModal.cpp"
            "ERS_GUI_CLASS_ModelRenameModal.cpp"
            "ERS_GUI_CLASS_DeleteScene.cpp"
            "ERS_GUI_CLASS_DeleteModel.cpp"
            "ERS_GUI_CLASS_DeletePointLight.cpp"
            "ERS_GUI_CLASS_DeleteDirectionalLight.cpp"
            "ERS_GUI_CLASS_DeleteSpotLight.cpp"
            "ERS_GUI_CLASS_ModelReplaceModal.cpp"


            # Add Header Files (.h)
            "GUI_Window_SceneTree.h"
            "ERS_GUI_FUNCTION_DuplicateModel.h"
            "ERS_GUI_FUNCTION_DuplicateScene.h"
            "ERS_GUI_FUNCTION_DuplicatePointLight.h"
            "ERS_GUI_FUNCTION_DuplicateDirectionalLight.h"
            "ERS_GUI_FUNCTION_DuplicateSpotLight.h"
            "ERS_GUI_CLASS_PointLightRenameModal.h"
            "ERS_GUI_CLASS_DirectionalLightRenameModal.h"
            "ERS_GUI_CLASS_SpotLightRenameModal.h"
            "ERS_GUI_CLASS_SceneRenameModal.h"
            "ERS_GUI_CLASS_ModelRenameModal.h"
            "ERS_GUI_CLASS_DeleteScene.h"
            "ERS_GUI_CLASS_DeleteModel.h"
            "ERS_GUI_CLASS_DeletePointLight.h"
            "ERS_GUI_CLASS_DeleteDirectionalLight.h"
            "ERS_GUI_CLASS_DeleteSpotLight.h"
            "ERS_GUI_CLASS_ModelReplaceModal.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_SceneTree
    glad
    glfw
    IMGUI
    )

# Link Internal Libs
target_link_libraries(GUI_Window_SceneTree
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_ProjectUtils
    ERS_STRUCT_SceneObject

    ERS_LoggingSystem
    
    ERS_Editor_3DCursor

    ERS_STRUCT_Scene
    ERS_SceneManager
    )

target_include_directories(GUI_Window_SceneTree PUBLIC ./)
