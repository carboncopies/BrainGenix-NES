########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_FramerateManager

            # Add Source Files (.cpp)
            "ERS_FramerateManager.cpp"

            # Add Header Files (.h)
            "ERS_FramerateManager.h"

            ${BACKWARD_ENABLE}
)

set_property(TARGET ERS_FramerateManager PROPERTY CXX_STANDARD 17)

# Link 3rd Party Libs
target_link_libraries(ERS_FramerateManager
    glad
    glfw
    )

# Link Internal Libs
target_link_libraries(ERS_FramerateManager
    )

target_include_directories(ERS_FramerateManager PUBLIC ./)

