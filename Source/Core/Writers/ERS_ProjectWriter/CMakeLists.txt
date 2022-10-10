########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_ProjectWriter

            # Add Source Files (.cpp)
            "ERS_ProjectWriter.cpp"

            # Add Header Files (.h)
            "ERS_ProjectWriter.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_ProjectWriter
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(ERS_ProjectWriter
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_Project
    ERS_STRUCT_IOData
    )

target_include_directories(ERS_ProjectWriter PUBLIC ./)

