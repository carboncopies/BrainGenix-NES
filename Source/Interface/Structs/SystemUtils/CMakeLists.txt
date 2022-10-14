########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(SystemUtils

            # Add Source Files (.cpp)
            "SystemUtils.cpp"

            # Add Header Files (.h)
            "SystemUtils.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(SystemUtils
    )

# Link Internal Libs
target_link_libraries(SystemUtils

    ERS_LoggingSystem
    HardwareInformation

    )


target_include_directories(SystemUtils PUBLIC ./)
