########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_STRUCT_SystemUtils

            # Add Source Files (.cpp)
            "ERS_STRUCT_SystemUtils.cpp"

            # Add Header Files (.h)
            "ERS_STRUCT_SystemUtils.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_STRUCT_SystemUtils
    )

# Link Internal Libs
target_link_libraries(ERS_STRUCT_SystemUtils

    ERS_InputOutputSubsystem
    ERS_FramerateManager

    ERS_LoggingSystem
    ERS_HardwareInformation
    ERS_CLASS_PythonInterpreterIntegration

    ERS_STRUCT_RendererSettings

    )


target_include_directories(ERS_STRUCT_SystemUtils PUBLIC ./)
