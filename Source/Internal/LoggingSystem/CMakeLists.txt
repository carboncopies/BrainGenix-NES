########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(LoggingSystem

            # Add Source Files (.cpp)
            "LoggingSystem.cpp"
            "RGBColor.cpp"
            "LogSystemInfo.cpp"

            # Add Header Files (.h)
            "LoggingSystem.h"
            "RGBColor.h"
            "LogSystemInfo.h"

            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(LoggingSystem
    yaml-cpp
    )

# Link Internal Libs
target_link_libraries(LoggingSystem
    VersioningSystem
    CompileTimeStamp
    CompilePlatformInfo
    )

if (WIN32) 
    ERSBuildLogger(${BoldGreen} "Build Warnings Not Enabled")
else ()
    target_compile_options(LoggingSystem PUBLIC -Wall -Wextra -Wpedantic)
    target_compile_options(LoggingSystem PUBLIC -Werror=uninitialized)
endif()

target_include_directories(LoggingSystem PUBLIC ./)

