########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(Renderer 

            # Add Source Files (.cpp)
            "RendererManager.cpp"

            # Add Header Files (.h)
            "RendererManager.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(Renderer
    yaml-cpp
    glm
    glad
    glfw
    FreeImage
    )

# Link Internal Libs
target_link_libraries(Renderer
    ERS_LoggingSystem
    ERS_CLASS_VisualRenderer

    VisualEditor

    ERS_Editor_3DCursor
    
    ERS_ModelWriter
    ERS_ModelLoader
    ERS_ShaderLoader
    
    ERS_STRUCT_IOData
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_ProjectUtils
    ERS_STRUCT_HumanInputDeviceUtils
    ERS_STRUCT_OpenGLDefaults

    ERS_VersioningSystem
    )

target_include_directories(Renderer PUBLIC ./)
