###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(BG-NES-LoggingSystem

            # Add Source Files (.cpp)
            "BG-NES-LoggingSystem.cpp"

            # Add Header Files (.h)
            "BG-NES-LoggingSystem.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(BG-NES-LoggingSystem
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(BG-NES-LoggingSystem

    )

target_include_directories(BG-NES-LoggingSystem PUBLIC ./)

