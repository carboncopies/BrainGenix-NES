###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(NES_ParentDirName

            # Add Source Files (.cpp)
            "NES_SourceName.cpp"

            # Add Header Files (.h)
            "NES_SourceName.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(NES_ParentDirName
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(NES_ParentDirName

    )

target_include_directories(NES_ParentDirName PUBLIC ./)