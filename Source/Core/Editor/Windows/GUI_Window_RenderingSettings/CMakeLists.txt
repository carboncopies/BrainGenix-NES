# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_RenderingSettings

            "GUI_Window_RenderingSettings.cpp"
            "GUI_Window_RenderingSettings.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_RenderingSettings
    glad
    glfw
    IMGUI
    )

# Link Internal Libs
target_link_libraries(GUI_Window_RenderingSettings
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_RendererSettings

    )

target_include_directories(GUI_Window_RenderingSettings PUBLIC ./)