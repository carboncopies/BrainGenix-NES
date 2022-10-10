########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_VersioningSystem

            # Add Source Files (.cpp)
            "ERS_Version.cpp"

            # Add Header Files (.h)
            "ERS_Version.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_VersioningSystem
    
    )

# Link Internal Libs
target_link_libraries(ERS_VersioningSystem
    
    )

target_include_directories(ERS_VersioningSystem PUBLIC ./)