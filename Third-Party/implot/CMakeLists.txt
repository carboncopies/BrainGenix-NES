# Create Library (Name Should Be Parent Dir Name)
add_library(ImPlot

            # Add Source Files (.cpp)
            "implot_items.cpp"
            "implot.cpp"

            # Add Header Files (.h)
            "implot_internal.h"
            "implot.h"

            ${BACKWARD_ENABLE}
            )

set_property(TARGET ImPlot PROPERTY CXX_STANDARD 11)

# Link 3rd Party Libs
target_link_libraries(ImPlot

    IMGUI
    )


target_include_directories(ImPlot PUBLIC ./)