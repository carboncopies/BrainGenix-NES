########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################

# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_AssetExplorer

            # Add Source Files (.cpp)
            "GUI_Window_AssetExplorer.cpp"
            "ERS_GUI_CLASS_ScriptRenameModal.cpp"

            # Add Header Files (.h)
            "GUI_Window_AssetExplorer.h"
            "ERS_GUI_CLASS_ScriptRenameModal.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_AssetExplorer
    glad
    glfw
    IMGUI
    )

# Link Internal Libs
target_link_libraries(GUI_Window_AssetExplorer
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_ProjectUtils
    )

target_include_directories(GUI_Window_AssetExplorer PUBLIC ./)
