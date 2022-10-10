########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_STRUCT_Project

            # Add Source Files (.cpp)
            "ERS_STRUCT_Project.cpp"

            # Add Header Files (.h)
            "ERS_STRUCT_Project.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_STRUCT_Project
    )

# Link Internal Libs
target_link_libraries(ERS_STRUCT_Project
    ERS_STRUCT_ControllerSettings
    ERS_STRUCT_ShaderProgramAssetIDs
    ERS_STRUCT_Script
    )


target_include_directories(ERS_STRUCT_Project PUBLIC ./)
set_target_properties(ERS_STRUCT_Project PROPERTIES LINKER_LANGUAGE CXX)