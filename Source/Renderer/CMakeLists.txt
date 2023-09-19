add_library(NESRenderer 

    "BG/Renderer/Interface.h"
    "BG/Renderer/Interface.cpp"
    "BG/Renderer/Manager.h"
    "BG/Renderer/Manager.cpp"
    "BG/Renderer/VulkanHelpers.h"
    "BG/Renderer/VulkanHelpers.cpp"
    "BG/Renderer/VulkanShaders.h"
    "BG/Renderer/VulkanShaders.cpp"
    "BG/Renderer/RenderData.h"
    "BG/Renderer/RenderData.cpp"

)

target_link_libraries(NESRenderer

    bg-common-logger

    vk-bootstrap::vk-bootstrap
    Vulkan::Vulkan
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    glm::glm
)
target_include_directories(NESRenderer PUBLIC ${SRC_DIR}/Renderer)