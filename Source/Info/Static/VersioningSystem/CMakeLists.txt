################################################################
# This file is part of the BrainGenix-API Unified API Endpoint #
################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(VersioningSystem

            # Add Source Files (.cpp)
            "Version.cpp"

            # Add Header Files (.h)
            "Version.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(VersioningSystem
    
    )

# Link Internal Libs
target_link_libraries(VersioningSystem
    
    )

target_include_directories(VersioningSystem PUBLIC ./)