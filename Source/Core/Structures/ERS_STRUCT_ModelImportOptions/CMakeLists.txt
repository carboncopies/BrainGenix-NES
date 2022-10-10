########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_STRUCT_ModelImportOptions

            # Add Source Files (.cpp)
            "ERS_STRUCT_ModelImportOptions.cpp"

            # Add Header Files (.h)
            "ERS_STRUCT_ModelImportOptions.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_STRUCT_ModelImportOptions
    )

# Link Internal Libs
target_link_libraries(ERS_STRUCT_ModelImportOptions
    )


target_include_directories(ERS_STRUCT_ModelImportOptions PUBLIC ./)
set_target_properties(ERS_STRUCT_ModelImportOptions PROPERTIES LINKER_LANGUAGE CXX)