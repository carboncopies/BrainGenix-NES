# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_FramerateGraph

            "GUI_Window_FramerateGraph.cpp"
            "GUI_Window_FramerateGraph.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_FramerateGraph
    IMGUI
    ImPlot
    )

# Link Internal Libs
target_link_libraries(GUI_Window_FramerateGraph
    )

target_include_directories(GUI_Window_FramerateGraph PUBLIC ./)