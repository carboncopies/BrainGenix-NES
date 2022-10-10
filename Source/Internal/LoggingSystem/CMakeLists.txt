########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_LoggingSystem

            # Add Source Files (.cpp)
            "ERS_LoggingSystem.cpp"
            "ERS_RGBColor.cpp"
            "ERS_LogSystemInfo.cpp"

            # Add Header Files (.h)
            "ERS_LoggingSystem.h"
            "ERS_RGBColor.h"
            "ERS_LogSystemInfo.h"

            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_LoggingSystem
    yaml-cpp
    glad
    )

# Link Internal Libs
target_link_libraries(ERS_LoggingSystem
    ERS_VersioningSystem
    ERS_CompileTimeStamp
    ERS_CompilePlatformInfo
    )

if (WIN32) 
    ERSBuildLogger(${BoldGreen} "Build Warnings Not Enabled")
else ()
    target_compile_options(ERS_LoggingSystem PUBLIC -Wall -Wextra -Wpedantic)
    target_compile_options(ERS_LoggingSystem PUBLIC -Werror=uninitialized)
endif()

target_include_directories(ERS_LoggingSystem PUBLIC ./)

