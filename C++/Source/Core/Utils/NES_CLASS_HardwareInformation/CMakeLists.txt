########################################################################
# This file is part of the BrainGenix-NES Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(NES_CLASS_HardwareInformation

            # Add Source Files (.cpp)
            "NES_CLASS_HardwareInformation.cpp"

            # Add Header Files (.h)
            "NES_CLASS_HardwareInformation.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(NES_CLASS_HardwareInformation
    infoware
    ${CMAKE_THREAD_LIBS_INIT}
    )

# Link Internal Libs
target_link_libraries(NES_CLASS_HardwareInformation

    NES_CLASS_LoggingSystem

    NES_STRUCT_HardwareInfo

    )

target_include_directories(NES_CLASS_HardwareInformation PUBLIC ./)