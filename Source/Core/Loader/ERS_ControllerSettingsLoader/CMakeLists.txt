########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_ControllerSettingsLoader

            # Add Source Files (.cpp)
            "ERS_ControllerSettingsLoader.cpp"

            # Add Header Files (.h)
            "ERS_ControllerSettingsLoader.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_ControllerSettingsLoader
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(ERS_ControllerSettingsLoader
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_IOData
    ERS_STRUCT_ControllerSettings
    )


target_include_directories(ERS_ControllerSettingsLoader PUBLIC ./)
