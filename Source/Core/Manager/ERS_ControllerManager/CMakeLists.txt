########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_ControllerInputManager

            # Add Source Files (.cpp)
            "ERS_ControllerInputManager.cpp"

            # Add Header Files (.h)
            "ERS_ControllerInputManager.h"

            ${BACKWARD_ENABLE}
)

#set_property(TARGET ERS_ControllerInputManager PROPERTY CXX_STANDARD 17)

# Link 3rd Party Libs
target_link_libraries(ERS_ControllerInputManager

    glad
    glfw

    )

# Link Internal Libs
target_link_libraries(ERS_ControllerInputManager

    ERS_STRUCT_SystemUtils
    ERS_STRUCT_ControllerSettings

    )

target_include_directories(ERS_ControllerInputManager PUBLIC ./)

