# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_RAMGraph

            "GUI_Window_RAMGraph.cpp"
            "GUI_Window_RAMGraph.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_RAMGraph
    IMGUI
    ImPlot
    )

# Link Internal Libs
target_link_libraries(GUI_Window_RAMGraph
    
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_HardwareInfo

    )

target_include_directories(GUI_Window_RAMGraph PUBLIC ./)