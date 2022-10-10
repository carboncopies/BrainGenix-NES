########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################

# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_ScriptEditor

            # Add Source Files (.cpp)
            "GUI_Window_ScriptEditor.cpp"

            # Add Header Files (.h)
            "GUI_Window_ScriptEditor.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_ScriptEditor
    IMGUI
    ImGuiColorTextEdit
    )

# Link Internal Libs
target_link_libraries(GUI_Window_ScriptEditor
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_ProjectUtils
    ERS_STRUCT_Script

    ERS_CLASS_VisualRenderer
    )

target_include_directories(GUI_Window_ScriptEditor PUBLIC ./)
