###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(NES_EmptyModel

            # Add Source Files (.cpp)
            "NES_EmptyModel.cpp"

            # Add Header Files (.h)
            "NES_EmptyModel.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(NES_EmptyModel
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(NES_EmptyModel

    )

target_include_directories(NES_EmptyModel PUBLIC ./)