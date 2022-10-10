########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_CompileTimeStamp

            # Add Source Files (.cpp)
            "ERS_CompileTimeStamp.cpp"

            # Add Header Files (.h)
            "ERS_CompileTimeStamp.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_CompileTimeStamp
    
    )

# Link Internal Libs
target_link_libraries(ERS_CompileTimeStamp
    
    )

target_include_directories(ERS_CompileTimeStamp PUBLIC ./)