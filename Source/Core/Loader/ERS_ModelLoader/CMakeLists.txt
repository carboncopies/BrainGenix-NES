########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_ModelLoader

            # Add Source Files (.cpp)
            "ERS_CLASS_ModelLoader.cpp"
            "ERS_CLASS_AssetStreamingManager.cpp"
            "ERS_CLASS_AsyncTextureUpdater.cpp"
            "ERS_CLASS_AssetStreamingSystemResourceMonitor.cpp"
            "ERS_FUNCTION_Legacy_PreprocessTextureInfo.cpp"
            "ERS_FUNCTION_ModelMetadataDecoderV0.0.0.cpp"
            "ERS_FUNCTION_ModelMetadataDecoderV0.0.1.cpp"
            "ERS_FUNCTION_ModelMetadataDecoderV0.0.2.cpp"
            "ERS_FUNCTION_ModelMetadataDecoderManager.cpp"

            # Add Header Files (.h)
            "ERS_CLASS_ModelLoader.h"
            "ERS_CLASS_AssetStreamingManager.h"
            "ERS_CLASS_AsyncTextureUpdater.h"
            "ERS_CLASS_AssetStreamingSystemResourceMonitor.h"
            "ERS_FUNCTION_Legacy_PreprocessTextureInfo.h"
            "ERS_FUNCTION_ModelMetadataDecoderV0.0.0.h"
            "ERS_FUNCTION_ModelMetadataDecoderV0.0.1.h"
            "ERS_FUNCTION_ModelMetadataDecoderV0.0.2.h"
            "ERS_FUNCTION_ModelMetadataDecoderManager.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_ModelLoader
    yaml-cpp
    glm
    assimp
    FreeImage
    ThreadNamingUtils
    )

# Link Internal Libs
target_link_libraries(ERS_ModelLoader
    ERS_STRUCT_Mesh
    ERS_STRUCT_Model
    ERS_STRUCT_Texture
    ERS_STRUCT_TextureLevel
    ERS_STRUCT_Vertex
    ERS_STRUCT_Scene

    ERS_STRUCT_SystemUtils
    ERS_STRUCT_Scene
    ERS_STRUCT_Camera
    ERS_STRUCT_HardwareInfo
    )

target_include_directories(ERS_ModelLoader PUBLIC ./)
