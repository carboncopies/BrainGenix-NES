########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_ProjectLoader

            # Add Source Files (.cpp)
            "ERS_ProjectLoader.cpp"

            # Add Header Files (.h)
            "ERS_ProjectLoader.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_ProjectLoader
    )

# Link Internal Libs
target_link_libraries(ERS_ProjectLoader
    ERS_STRUCT_IOData
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_Project
    ERS_STRUCT_ShaderProgramAssetIDs
    ERS_STRUCT_Script

    ERS_InputOutputSubsystem

    )


target_include_directories(ERS_ProjectLoader PUBLIC ./)
