########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(PARENT_DIR_NAME

            # Add Source Files (.cpp)
            "YOURSOURCE.cpp"

            # Add Header Files (.h)
            "YOURSOURCE.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(PARENT_DIR_NAME
    )

# Link Internal Libs
target_link_libraries(PARENT_DIR_NAME
    ERS_CLASS_LoggingSystem
    )

target_include_directories(PARENT_DIR_NAME PUBLIC ./)