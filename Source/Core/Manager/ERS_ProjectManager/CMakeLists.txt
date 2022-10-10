########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_ProjectManager

            # Add Source Files (.cpp)
            "ERS_ProjectManager.cpp"

            # Add Header Files (.h)
            "ERS_ProjectManager.h"

            ${BACKWARD_ENABLE}
)

set_property(TARGET ERS_ProjectManager PROPERTY CXX_STANDARD 17)

# Link 3rd Party Libs
target_link_libraries(ERS_ProjectManager
    )

# Link Internal Libs
target_link_libraries(ERS_ProjectManager
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_Project

    ERS_SceneLoader
    ERS_SceneManager
    ERS_ProjectLoader
    ERS_ProjectWriter
    ERS_ControllerSettingsLoader
    )

target_include_directories(ERS_ProjectManager PUBLIC ./)

