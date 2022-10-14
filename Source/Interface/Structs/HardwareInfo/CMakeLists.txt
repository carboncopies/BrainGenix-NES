########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(HardwareInfo

            # Add Source Files (.cpp)
            "HardwareInfo.cpp"

            # Add Header Files (.h)
            "HardwareInfo.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(HardwareInfo
    )

# Link Internal Libs
target_link_libraries(HardwareInfo
    )


target_include_directories(HardwareInfo PUBLIC ./)
set_target_properties(HardwareInfo PROPERTIES LINKER_LANGUAGE CXX)