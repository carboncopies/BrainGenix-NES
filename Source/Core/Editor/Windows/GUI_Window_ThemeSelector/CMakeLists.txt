########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################



# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_ThemeSelector

            # Add Source Files (.cpp)
            "GUI_Window_ThemeSelector.cpp"

            # Add Header Files (.h)
            "GUI_Window_ThemeSelector.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_ThemeSelector
    IMGUI
    )

# Link Internal Libs
target_link_libraries(GUI_Window_ThemeSelector
    ERS_Editor_ThemeManager
    )

target_include_directories(GUI_Window_ThemeSelector PUBLIC ./)
