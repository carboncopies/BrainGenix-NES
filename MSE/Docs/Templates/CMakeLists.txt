###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(BG-NES-ParentDirName

            # Add Source Files (.cpp)
            "BG-NES-SourceName.cpp"

            # Add Header Files (.h)
            "BG-NES-SourceName.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(BG-NES-ParentDirName
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(BG-NES-ParentDirName

    )

target_include_directories(BG-NES-ParentDirName PUBLIC ./)