########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(HardwareInformation

            # Add Source Files (.cpp)
            "HardwareInformation.cpp"

            # Add Header Files (.h)
            "HardwareInformation.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(HardwareInformation
    infoware
    ThreadNamingUtils
    ${CMAKE_THREAD_LIBS_INIT}
    )

# Link Internal Libs
target_link_libraries(HardwareInformation

    ERS_LoggingSystem

    HardwareInfo

    )

target_include_directories(HardwareInformation PUBLIC ./)