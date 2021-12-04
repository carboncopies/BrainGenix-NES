###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(NES_EmptyNeuronModel

            # Add Source Files (.cpp)
            "NES_EmptyNeuronModel.cpp"

            # Add Header Files (.h)
            "NES_EmptyNeuronModel.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(NES_EmptyNeuronModel
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(NES_EmptyNeuronModel

    )

target_include_directories(NES_EmptyNeuronModel PUBLIC ./)