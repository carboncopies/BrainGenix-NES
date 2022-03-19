###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(NES_CLASS_LoggingSystem

            # Add Source Files (.cpp)
            "NES_CLASS_LoggingSystem.cpp"

            # Add Header Files (.h)
            "NES_CLASS_LoggingSystem.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(NES_CLASS_LoggingSystem
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(NES_CLASS_LoggingSystem
    NES_STRUCT_RGBColor
    )

if (WIN32) 
    NESBuildLogger(${BoldGreen} "Build Warnings Not Enabled")
else ()
    target_compile_options(NES_CLASS_LoggingSystem PUBLIC -Wall -Wextra -Wpedantic)
    target_compile_options(NES_CLASS_LoggingSystem PUBLIC -Werror=uninitialized)
endif()

target_include_directories(NES_CLASS_LoggingSystem PUBLIC ./)

