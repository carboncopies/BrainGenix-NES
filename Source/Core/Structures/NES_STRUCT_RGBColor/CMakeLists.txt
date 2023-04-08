###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(NES_STRUCT_RGBColor

            # Add Header Files (.cpp)
            "NES_STRUCT_RGBColor.cpp"

            # Add Header Files (.h)
            "NES_STRUCT_RGBColor.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(NES_STRUCT_RGBColor
    )

# Link Internal Libs
target_link_libraries(NES_STRUCT_RGBColor
    )


target_include_directories(NES_STRUCT_RGBColor PUBLIC ./)
