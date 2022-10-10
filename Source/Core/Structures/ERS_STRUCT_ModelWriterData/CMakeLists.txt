########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(ERS_STRUCT_ModelWriterData 

            # Add Source Files (.cpp)
            "ERS_STRUCT_ModelWriterData.cpp"

            # Add Header Files (.h)
            "ERS_STRUCT_ModelWriterData.h"
            
            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries(ERS_STRUCT_ModelWriterData
    assimp
    )

# Link Internal Libs
target_link_libraries(ERS_STRUCT_ModelWriterData
    ERS_STRUCT_Model
    )

target_include_directories(ERS_STRUCT_ModelWriterData PUBLIC ./)
set_target_properties(ERS_STRUCT_ModelWriterData PROPERTIES LINKER_LANGUAGE CXX)