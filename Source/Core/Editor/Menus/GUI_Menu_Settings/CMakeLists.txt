########################################################################
# This File is part of the BrainGenix-ERS Environment Rendering System #
########################################################################

# Create Library (Name Should Be Parent Dir Name)
add_library(Menu_Settings

            # Add Source Settingss (.cpp)
            "GUI_Menu_Settings.cpp"

            # Add Header Settingss (.h)
            "GUI_Menu_Settings.h"


            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(Menu_Settings
    IMGUI
    )

# Link Internal Libs
target_link_libraries(Menu_Settings
    ERS_STRUCT_SystemUtils
    ERS_STRUCT_HumanInputDeviceUtils
    ERS_STRUCT_ProjectUtils

    ERS_Editor_WindowManager

    GUI_Window_ControllerSettings

    )

target_include_directories(Menu_Settings PUBLIC ./)
