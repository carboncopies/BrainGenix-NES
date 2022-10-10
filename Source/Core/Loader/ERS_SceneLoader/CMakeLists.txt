########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_SceneLoader

            # Add Source Files (.cpp)
            "ERS_SceneLoader.cpp"
            "ERS_FUNCTION_YAMLHelpers.cpp"
            "ERS_FUNCTION_SceneDecoderManager.cpp"
            "ERS_FUNCTION_SceneDecoderV1.cpp"
            "ERS_FUNCTION_SceneDecoderV2.cpp"
            "ERS_FUNCTION_SceneDecoderV3.cpp"
            "ERS_FUNCTION_SceneDecoderV4.cpp"
            "ERS_FUNCTION_SceneDecoderV5.cpp"
            "ERS_FUNCTION_SceneDecoderV6.cpp"

            # Add Header Files (.h)
            "ERS_SceneLoader.h"
            "ERS_FUNCTION_YAMLHelpers.h"
            "ERS_FUNCTION_SceneDecoderManager.h"
            "ERS_FUNCTION_SceneDecoderV1.h"
            "ERS_FUNCTION_SceneDecoderV2.h"
            "ERS_FUNCTION_SceneDecoderV3.h"
            "ERS_FUNCTION_SceneDecoderV4.h"
            "ERS_FUNCTION_SceneDecoderV5.h"
            "ERS_FUNCTION_SceneDecoderV6.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_SceneLoader
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(ERS_SceneLoader

    ERS_ModelLoader

    ERS_LoggingSystem

    ERS_STRUCT_Model
    ERS_STRUCT_Scene
    ERS_STRUCT_Mesh
    ERS_STRUCT_Texture
    ERS_STRUCT_Shader
    ERS_STRUCT_Light
    ERS_STRUCT_SceneCamera

    )

target_include_directories(ERS_SceneLoader PUBLIC ./)

