################################################################
# This file is part of the BrainGenix-API Unified API Endpoint #
################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(CompilePlatformInfo

            # Add Source Files (.cpp)
            "CompilePlatformInfo.cpp"

            # Add Header Files (.h)
            "CompilePlatformInfo.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(CompilePlatformInfo
    
    )

# Link Internal Libs
target_link_libraries(CompilePlatformInfo
    
    )

target_include_directories(CompilePlatformInfo PUBLIC ./)