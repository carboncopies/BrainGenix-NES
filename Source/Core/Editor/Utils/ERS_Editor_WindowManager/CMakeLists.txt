########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_Editor_WindowManager

            # Add Source Files (.cpp)
            "ERS_Editor_WindowManager.cpp"
            "ERS_STRUCT_Windows.cpp"

            # Add Header Files (.h)
            "ERS_Editor_WindowManager.h"
            "ERS_STRUCT_Windows.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_Editor_WindowManager
    )

# Link Internal Libs
target_link_libraries(ERS_Editor_WindowManager
    ERS_STRUCT_SystemUtils    


    GUI_Window_About
    GUI_Window_AssetExplorer
    GUI_Window_ControllerSettings
    GUI_Window_FrameLatencyGraph
    GUI_Window_FramerateCounter
    GUI_Window_FramerateGraph
    GUI_Window_FramerateHistogram
    GUI_Window_FrameratePlot
    GUI_Window_ImportProgressBar
    GUI_Window_ObjectProperties
    GUI_Window_ProjectSettings
    GUI_Window_RAMGraph
    GUI_Window_RenderingSettings
    GUI_Window_EditorCameraSettings
    GUI_Window_SceneTree
    GUI_Window_ScriptEditor
    GUI_Window_ShaderEditor
    GUI_Window_SystemLog
    GUI_Window_TestEditor
    GUI_Window_ThemeSelector
    GUI_Window_FontSelector
    GUI_Window_OpenProject
    GUI_Window_NewProject
    GUI_Window_AssetStreamingSettings

    GUI_Window_ImportModel
    GUI_Window_ImportModelDirectory

    )

target_include_directories(ERS_Editor_WindowManager PUBLIC ./)