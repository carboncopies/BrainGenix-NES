########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_STRUCT_Camera 

            # Add Source Files (.cpp)
            "ERS_STRUCT_Camera.cpp"

            # Add Header Files (.h)
            "ERS_STRUCT_Camera.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_STRUCT_Camera
    glm
    glad
    )

# Link Internal Libs
target_link_libraries(ERS_STRUCT_Camera
    )

target_include_directories(ERS_STRUCT_Camera PUBLIC ./)