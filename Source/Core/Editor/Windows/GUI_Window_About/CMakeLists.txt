########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################

# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_About

            # Add Source Files (.cpp)
            "GUI_Window_About.cpp"

            # Add Header Files (.h)
            "GUI_Window_About.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_About
    glad
    glfw
    IMGUI
    )

# Link Internal Libs
target_link_libraries(GUI_Window_About
    ERS_STRUCT_SystemUtils

    ERS_VersioningSystem
    ERS_CompilePlatformInfo
    ERS_CompileTimeStamp
    )

target_include_directories(GUI_Window_About PUBLIC ./)
