########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################



# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_FontSelector

            # Add Source Files (.cpp)
            "GUI_Window_FontSelector.cpp"

            # Add Header Files (.h)
            "GUI_Window_FontSelector.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_FontSelector
    IMGUI
    )

# Link Internal Libs
target_link_libraries(GUI_Window_FontSelector
    ERS_Editor_FontManager
    )

target_include_directories(GUI_Window_FontSelector PUBLIC ./)
