########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################

# Create Library (Name Should Be Parent Dir Name)
add_library(Menu_File

            # Add Source Files (.cpp)
            "GUI_Menu_File.cpp"

            # Add Header Files (.h)
            "GUI_Menu_File.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(Menu_File
    IMGUI
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(Menu_File
    ERS_LoggingSystem
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_ProjectUtils

    ERS_Editor_WindowManager
    
    ERS_ProjectLoader
    ERS_SceneWriter
    ERS_SceneManager

    ERS_STRUCT_Scene

    GUI_Window_ProjectSettings

    ERS_CLASS_VisualRenderer
    )

target_include_directories(Menu_File PUBLIC ./)
