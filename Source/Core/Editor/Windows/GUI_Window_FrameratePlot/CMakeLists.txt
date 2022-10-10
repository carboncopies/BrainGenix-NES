# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_FrameratePlot

            "GUI_Window_FrameratePlot.cpp"
            "GUI_Window_FrameratePlot.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_FrameratePlot
    IMGUI
    )

# Link Internal Libs
target_link_libraries(GUI_Window_FrameratePlot
    )

target_include_directories(GUI_Window_FrameratePlot PUBLIC ./)