# Create Library (Name Should Be Parent Dir Name)
add_library(ImGuizmo

            # Add Source Files (.cpp)
            "GraphEditor.cpp"
            "ImCurveEdit.cpp"
            "ImGradient.cpp"
            "ImGuizmo.cpp"
            "ImSequencer.cpp"

            # Add Header Files (.h)
            "GraphEditor.h"
            "ImCurveEdit.h"
            "ImGradient.h"
            "ImGuizmo.h"
            "ImSequencer.h"
            "ImZoomSlider.h"
                
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ImGuizmo

    IMGUI
    )


target_include_directories(ImGuizmo PUBLIC ./)