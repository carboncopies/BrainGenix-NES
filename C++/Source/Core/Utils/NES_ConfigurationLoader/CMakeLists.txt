###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(NES_ConfigurationLoader

            # Add Source Files (.cpp)
            "NES_LocalConfigurationLoader.cpp"

            # Add Header Files (.h)
            "NES_LocalConfigurationLoader.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(NES_ConfigurationLoader
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(NES_ConfigurationLoader

    )

target_include_directories(NES_ConfigurationLoader PUBLIC ./)