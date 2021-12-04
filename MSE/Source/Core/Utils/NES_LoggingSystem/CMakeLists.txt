###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(NES_LoggingSystem

            # Add Source Files (.cpp)
            "NES_LoggingSystem.cpp"

            # Add Header Files (.h)
            "NES_LoggingSystem.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(NES_LoggingSystem
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(NES_LoggingSystem

    )

target_include_directories(NES_LoggingSystem PUBLIC ./)

