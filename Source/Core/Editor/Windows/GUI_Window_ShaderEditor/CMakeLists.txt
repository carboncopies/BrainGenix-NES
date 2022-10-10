########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################

# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_ShaderEditor

            # Add Source Files (.cpp)
            "GUI_Window_ShaderEditor.cpp"

            # Add Header Files (.h)
            "GUI_Window_ShaderEditor.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_ShaderEditor
    IMGUI
    ImGuiColorTextEdit
    )

# Link Internal Libs
target_link_libraries(GUI_Window_ShaderEditor
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_ProjectUtils
    ERS_STRUCT_Shader
    ERS_STRUCT_ShaderProgramAssetIDs

    ERS_ShaderLoader

    ERS_CLASS_VisualRenderer
    )

target_include_directories(GUI_Window_ShaderEditor PUBLIC ./)
