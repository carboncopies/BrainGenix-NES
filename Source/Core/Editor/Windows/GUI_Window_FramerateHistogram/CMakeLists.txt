# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_FramerateHistogram

            "GUI_Window_FramerateHistogram.cpp"
            "GUI_Window_FramerateHistogram.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_FramerateHistogram
    IMGUI
    )

# Link Internal Libs
target_link_libraries(GUI_Window_FramerateHistogram
    )

target_include_directories(GUI_Window_FramerateHistogram PUBLIC ./)