########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_ExternalModelLoader

            # Add Source Files (.cpp)
            "ERS_ExternalModelLoader.cpp"

            # Add Header Files (.h)
            "ERS_ExternalModelLoader.h"
            
            ${BACKWARD_ENABLE}
            )
set_property(TARGET ERS_ExternalModelLoader PROPERTY CXX_STANDARD 17)

# Link 3rd Party Libs
target_link_libraries(ERS_ExternalModelLoader
    yaml-cpp
    glm
    assimp
    FreeImage
    Lucifer
    )

# Link Internal Libs
target_link_libraries(ERS_ExternalModelLoader
    ERS_STRUCT_Mesh
    ERS_STRUCT_Model
    ERS_STRUCT_Vertex
    ERS_STRUCT_ModelWriterData
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_IOData
    )

target_include_directories(ERS_ExternalModelLoader PUBLIC ./)