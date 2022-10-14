########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ArgumentParser

            # Add Source Files (.cpp)
            "ArgumentParser.cpp"

            # Add Header Files (.h)
            "ArgumentParser.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ArgumentParser
    ERS_LoggingSystem
    )

# Link Internal Libs
target_link_libraries(ArgumentParser
    )

target_include_directories(ArgumentParser PUBLIC ./)