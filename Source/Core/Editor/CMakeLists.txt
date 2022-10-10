########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(VisualEditor

            # Add Source Files (.cpp)
            "GUI.cpp"

            # Add Header Files (.h)
            "GUI.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(VisualEditor
    glad
    glfw
    IMGUI
    ImPlot
    ImGuizmo
    )

# Link Internal Libs
target_link_libraries(VisualEditor
    ERS_LoggingSystem
    ERS_CLASS_VisualRenderer
    
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_ProjectUtils
    ERS_STRUCT_HumanInputDeviceUtils

    ERS_Editor_ThemeManager
    ERS_Editor_FontManager
    ERS_Editor_UserProfileManager
    ERS_Editor_3DCursor
    ERS_Editor_WindowManager

    ERS_SceneManager

    Menu_File
    Menu_Window
    Menu_Debug
    Menu_Settings

    )

target_include_directories(VisualEditor PUBLIC ./)