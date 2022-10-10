########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_ShaderLoader

            # Add Source Files (.cpp)
            "ERS_CLASS_ShaderLoader.cpp"

            # Add Header Files (.h)
            "ERS_CLASS_ShaderLoader.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_ShaderLoader
    )

# Link Internal Libs
target_link_libraries(ERS_ShaderLoader
    ERS_STRUCT_Shader
    ERS_STRUCT_IOData
    ERS_STRUCT_SystemUtils
    )

target_include_directories(ERS_ShaderLoader PUBLIC ./)
