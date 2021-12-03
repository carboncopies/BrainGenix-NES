###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(BG-NES-ConfigurationLoader

            # Add Source Files (.cpp)
            "BG-NES-LocalConfigurationLoader.cpp"

            # Add Header Files (.h)
            "BG-NES-LocalConfigurationLoader.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(BG-NES-ConfigurationLoader
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(BG-NES-ConfigurationLoader

    )

target_include_directories(BG-NES-ConfigurationLoader PUBLIC ./)