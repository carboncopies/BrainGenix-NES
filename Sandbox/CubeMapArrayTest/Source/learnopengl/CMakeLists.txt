########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################


# Create Library (Name Should Be Parent Dir Name)
add_library(learnopengl
            # Add Source Files (.cpp)
            "camera.h"
            "model.h"
            "shader.h"
            "mesh.h"

            ${BACKWARD_ENABLE}
)

set_target_properties(learnopengl PROPERTIES LINKER_LANGUAGE CXX)

# Link 3rd Party Libs
target_link_libraries(learnopengl

    glad
    glfw

    )

# Link Internal Libs
target_link_libraries(learnopengl

    assimp

    )

target_include_directories(learnopengl PUBLIC ./)

