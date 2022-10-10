########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_CLASS_PythonInterpreterIntegration 

            # Add Source Files (.cpp)
            "ERS_CLASS_PythonInterpreterIntegration.cpp"
            "PyBind11ModelClass.cpp"
            "PyBind11SystemInfo.cpp"
            "PyBind11PointLightClass.cpp"
            "PyBind11DirectionalLightClass.cpp"
            "PyBind11SpotLightClass.cpp"
            

            # Add Header Files (.h)
            "ERS_CLASS_PythonInterpreterIntegration.h"
            "PyBind11ModelClass.h"
            "PyBind11SystemInfo.h"
            "PyBind11PointLightClass.h"
            "PyBind11DirectionalLightClass.h"
            "PyBind11SpotLightClass.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_CLASS_PythonInterpreterIntegration
    pybind11::embed
    )

# Link Internal Libs
target_link_libraries(ERS_CLASS_PythonInterpreterIntegration
    ERS_LoggingSystem

    ERS_STRUCT_Model
    ERS_STRUCT_Light

    )

target_include_directories(ERS_CLASS_PythonInterpreterIntegration PUBLIC ./)