# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_FrameLatencyGraph

            "GUI_Window_FrameLatencyGraph.cpp"
            "GUI_Window_FrameLatencyGraph.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_FrameLatencyGraph
    IMGUI
    ImPlot
    )

# Link Internal Libs
target_link_libraries(GUI_Window_FrameLatencyGraph
    ERS_STRUCT_SystemUtils
    )

target_include_directories(GUI_Window_FrameLatencyGraph PUBLIC ./)