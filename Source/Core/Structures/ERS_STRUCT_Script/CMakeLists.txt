########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_STRUCT_Script

            # Add Source Files (.cpp)
            "ERS_STRUCT_Script.cpp"

            # Add Header Files (.h)
            "ERS_STRUCT_Script.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_STRUCT_Script
    )

# Link Internal Libs
target_link_libraries(ERS_STRUCT_Script
    )


target_include_directories(ERS_STRUCT_Script PUBLIC ./)
set_target_properties(ERS_STRUCT_Script PROPERTIES LINKER_LANGUAGE CXX)