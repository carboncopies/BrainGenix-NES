# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_EditorCameraSettings

            "GUI_Window_EditorCameraSettings.cpp"
            "GUI_Window_EditorCameraSettings.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_EditorCameraSettings
    glad
    glfw
    IMGUI
    )

# Link Internal Libs
target_link_libraries(GUI_Window_EditorCameraSettings
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_RendererSettings

    ERS_CLASS_VisualRenderer

    )

target_include_directories(GUI_Window_EditorCameraSettings PUBLIC ./)