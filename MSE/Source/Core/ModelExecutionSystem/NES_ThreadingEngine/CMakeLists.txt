###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################

# Add Threads Lib
find_package(Threads REQUIRED)

# Create Library (Name Should Be Parent Dir Name)
add_library(NES_ThreadingEngine

            # Add Source Files (.cpp)
            "NES_ThreadingEngine.cpp"

            # Add Header Files (.h)
            "NES_ThreadingEngine.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(NES_ThreadingEngine
    ${CMAKE_THREAD_LIBS_INIT}
    )

# Link Internal Libs
target_link_libraries(NES_ThreadingEngine

    )

target_include_directories(NES_ThreadingEngine PUBLIC ./)