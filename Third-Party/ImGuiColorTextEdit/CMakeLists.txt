# Create Library (Name Should Be Parent Dir Name)
add_library(ImGuiColorTextEdit

            # Add Source Files (.cpp)
            "TextEditor.cpp"

            # Add Header Files (.h)
            "TextEditor.h"
                
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ImGuiColorTextEdit

    IMGUI
    )


target_include_directories(ImGuiColorTextEdit PUBLIC ./)