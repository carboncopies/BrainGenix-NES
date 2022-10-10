########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(Menu_Window

            # Add Source Files (.cpp)
            "GUI_Menu_Window.cpp"

            # Add Header Files (.h)
            "GUI_Menu_Window.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(Menu_Window
    IMGUI
    )

# Link Internal Libs
target_link_libraries(Menu_Window
    ERS_CLASS_VisualRenderer

    ERS_STRUCT_SystemUtils
    ERS_STRUCT_ProjectUtils

    ERS_Editor_WindowManager

    
    GUI_Window_SceneTree
    GUI_Window_SystemLog
    GUI_Window_AssetExplorer
    GUI_Window_ScriptEditor

    GUI_Window_RenderingSettings
    GUI_Window_RAMGraph
    GUI_Window_ObjectProperties
    GUI_Window_FramerateGraph
    GUI_Window_FrameLatencyGraph
    GUI_Window_FramerateCounter
    GUI_Window_FramerateHistogram
    GUI_Window_FrameratePlot
    GUI_Window_ShaderEditor
    GUI_Window_AssetStreamingSettings


    )

target_include_directories(Menu_Window PUBLIC ./)
