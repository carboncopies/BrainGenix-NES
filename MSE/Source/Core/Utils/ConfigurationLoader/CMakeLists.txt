########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ConfigurationLoader

            # Add Source Files (.cpp)

            # Add Header Files (.h)
            "LocalConfigurationLoader.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ConfigurationLoader

    )

# Link Internal Libs
target_link_libraries(ConfigurationLoader

    )

target_include_directories(ConfigurationLoader PUBLIC ./)