########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_CompilePlatformInfo

            # Add Source Files (.cpp)
            "ERS_CompilePlatformInfo.cpp"

            # Add Header Files (.h)
            "ERS_CompilePlatformInfo.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_CompilePlatformInfo
    
    )

# Link Internal Libs
target_link_libraries(ERS_CompilePlatformInfo
    
    )

target_include_directories(ERS_CompilePlatformInfo PUBLIC ./)