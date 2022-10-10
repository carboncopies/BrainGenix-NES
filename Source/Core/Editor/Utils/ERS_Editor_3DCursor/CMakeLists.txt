########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_Editor_3DCursor

            # Add Source Files (.cpp)
            "ERS_Editor_3DCursor.cpp"

            # Add Header Files (.h)
            "ERS_Editor_3DCursor.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_Editor_3DCursor
    glm
    IMGUI
    ImGuizmo
    )

# Link Internal Libs
target_link_libraries(ERS_Editor_3DCursor
    ERS_LoggingSystem
    
    ERS_STRUCT_Camera
    ERS_STRUCT_LocRotScale
    )

target_include_directories(ERS_Editor_3DCursor PUBLIC ./)