###################################################################
# This file is part of the BrainGenix-NES Neuron Emulation System #
###################################################################


# Define Variables
set(LIBRARY_NAME "ERS_STRUCT_RGBColor")
set(LIBRARY_SOURCES

    # Add Header Files (.cpp)
    "NES_STRUCT_RGBColor.cpp"

    # Add Header Files (.h)
    "NES_STRUCT_RGBColor.h"

    )
set(LIBRARY_TEST_SOURCES

    # Add Header Files (.cpp)
    "RGBColorTest.cpp"
    
    # Add Header Files (.h)
    "RGBColorTest.h"

)
set(LIBRARY_TEST_NAME "Tests RGB Color Struct Values")

# Create Library (Name Should Be Parent Dir Name)
add_library(${LIBRARY_NAME}


            ${LIBRARY_SOURCES}
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(${LIBRARY_NAME}
    )

# Link Internal Libs
target_link_libraries(${LIBRARY_NAME}
    )


target_include_directories(${LIBRARY_NAME} PUBLIC ./)



# Unit Test Executable
add_executable("${LIBRARY_NAME}_Test" ${LIBRARY_TEST_SOURCES})
target_link_libraries("${LIBRARY_NAME}_Test" ${LIBRARY_NAME})
add_test(NAME "${LIBRARY_NAME} ${LIBRARY_TEST_NAME}" COMMAND "./${LIBRARY_NAME}_Test")