########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_STRUCT_ProjectUtils

            # Add Source Files (.cpp)
            "ERS_STRUCT_ProjectUtils.cpp"

            # Add Header Files (.h)
            "ERS_STRUCT_ProjectUtils.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_STRUCT_ProjectUtils
    )

# Link Internal Libs
target_link_libraries(ERS_STRUCT_ProjectUtils
    ERS_LoggingSystem
    ERS_SceneManager

    ERS_InputOutputSubsystem
    ERS_FramerateManager
    ERS_ModelWriter
    ERS_ModelLoader
    ERS_SceneLoader
    ERS_ProjectLoader
    ERS_ProjectManager
    ERS_ProjectWriter
    ERS_ModelImporter

    )


target_include_directories(ERS_STRUCT_ProjectUtils PUBLIC ./)
