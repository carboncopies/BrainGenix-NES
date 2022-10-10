########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################

# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_TestEditor

            # Add Source Files (.cpp)
            "GUI_Window_TestEditor.cpp"

            # Add Header Files (.h)
            "GUI_Window_TestEditor.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_TestEditor
    IMGUI
    ImGuiColorTextEdit
    )

# Link Internal Libs
target_link_libraries(GUI_Window_TestEditor
    ERS_STRUCT_SystemUtils
    )

target_include_directories(GUI_Window_TestEditor PUBLIC ./)
