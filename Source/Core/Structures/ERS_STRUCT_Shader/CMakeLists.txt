########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_STRUCT_Shader

            # Add Source Files (.cpp)
            "ERS_STRUCT_Shader.cpp"

            # Add Header Files (.h)
            "ERS_STRUCT_Shader.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_STRUCT_Shader
    glm
    glad
    glfw
    )

# Link Internal Libs
target_link_libraries(ERS_STRUCT_Shader
    ERS_LoggingSystem
    )

target_include_directories(ERS_STRUCT_Shader PUBLIC ./)