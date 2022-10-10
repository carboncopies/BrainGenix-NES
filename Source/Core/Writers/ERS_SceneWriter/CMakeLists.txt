########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_SceneWriter

            # Add Source Files (.cpp)
            "ERS_SceneWriter.cpp"

            # Add Header Files (.h)
            "ERS_SceneWriter.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_SceneWriter
    )

# Link Internal Libs
target_link_libraries(ERS_SceneWriter
    ERS_LoggingSystem
    ERS_STRUCT_Model
    ERS_STRUCT_Scene
    ERS_STRUCT_Mesh
    ERS_STRUCT_Texture

    ERS_STRUCT_SystemUtils
    ERS_STRUCT_IOData
    )

target_include_directories(ERS_SceneWriter PUBLIC ./)

