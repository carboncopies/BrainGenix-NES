###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Define Variables
set(LIBRARY_NAME "ERS_STRUCT_RGBColor")


# Create Library (Name Should Be Parent Dir Name)
add_library(${LIBRARY_NAME}

            # Add Header Files (.cpp)
            "NES_STRUCT_RGBColor.cpp"

            # Add Header Files (.h)
            "NES_STRUCT_RGBColor.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(${LIBRARY_NAME}
    )

# Link Internal Libs
target_link_libraries(${LIBRARY_NAME}
    )


target_include_directories(${LIBRARY_NAME} PUBLIC ./)





add_executable("AddTwoNumbers_Test" AddTwoNumbersTest.cpp)
target_link_libraries("AddTwoNumbers_Test" Submodule)

add_test(NAME "Test AddTwoNumbers Function" COMMAND "./AddTwoNumbers_Test")