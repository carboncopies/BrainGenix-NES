# Create Library (Name Should Be Parent Dir Name)
add_library(GUI_Window_ObjectProperties

            "GUI_Window_ObjectProperties.cpp"
            "GUI_Window_ObjectProperties.h"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(GUI_Window_ObjectProperties
    glad
    glfw
    IMGUI
    )

# Link Internal Libs
target_link_libraries(GUI_Window_ObjectProperties
        
    ERS_Editor_3DCursor

    ERS_STRUCT_ProjectUtils
    ERS_STRUCT_LocRotScale

    ERS_CLASS_VisualRenderer
    
    ERS_SceneManager 

    )

target_include_directories(GUI_Window_ObjectProperties PUBLIC ./)