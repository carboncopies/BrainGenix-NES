########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_STRUCT_Viewport

            # Add Source Files (.cpp)
            "ERS_STRUCT_Viewport.cpp"

            # Add Header Files (.h)
            "ERS_STRUCT_Viewport.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_STRUCT_Viewport
    )

# Link Internal Libs
target_link_libraries(ERS_STRUCT_Viewport
    ERS_STRUCT_Camera
    ERS_CLASS_VisualRenderer
    )


target_include_directories(ERS_STRUCT_Viewport PUBLIC ./)
