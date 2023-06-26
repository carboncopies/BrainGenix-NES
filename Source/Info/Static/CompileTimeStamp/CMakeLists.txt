################################################################
# This file is part of the BrainGenix-API Unified API Endpoint #
################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(CompileTimeStamp

            # Add Source Files (.cpp)
            "CompileTimeStamp.cpp"

            # Add Header Files (.h)
            "CompileTimeStamp.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(CompileTimeStamp
    
    )

# Link Internal Libs
target_link_libraries(CompileTimeStamp
    
    )

target_include_directories(CompileTimeStamp PUBLIC ./)