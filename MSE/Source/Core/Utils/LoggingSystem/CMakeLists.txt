########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################



# Specify CPP Files In Source (Name Should Start With Parent Dir Name)
set(LoggingSystem_Sources
    "LoggingSystem.cpp"
)

# Specify Header Files In Source (Name Should Start With Parent Dir Name)
set(LoggingSystem_Headers
    "LoggingSystem.h"
)


# Create Library (Name Should Be Parent Dir Name)
add_library(LoggingSystem ${LoggingSystem_Sources} ${LoggingSystem_Headers} ${BACKWARD_ENABLE})
target_link_libraries(LoggingSystem 
    yaml-cpp
    )
target_include_directories(LoggingSystem PUBLIC ./)

