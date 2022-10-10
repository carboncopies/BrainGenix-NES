########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_STRUCT_DepthMap

            # Add Source Files (.cpp)
            "ERS_STRUCT_DepthMap.cpp"

            # Add Header Files (.h)
            "ERS_STRUCT_DepthMap.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_STRUCT_DepthMap
    glm
    )

# Link Internal Libs
target_link_libraries(ERS_STRUCT_DepthMap
    )


target_include_directories(ERS_STRUCT_DepthMap PUBLIC ./)
set_target_properties(ERS_STRUCT_DepthMap PROPERTIES LINKER_LANGUAGE CXX)