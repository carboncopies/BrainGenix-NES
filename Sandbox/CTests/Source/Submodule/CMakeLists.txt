#Create Library (Name Should Be Parent Dir Name)
add_library(Submodule

            # Add Source Files (.cpp)
            "AddTwoNumbers.cpp"

            # Add Header Files (.h)
            "AddTwoNumbers.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(Submodule
    )

# Link Internal Libs
target_link_libraries(Submodule
    )

target_include_directories(Submodule PUBLIC ./)