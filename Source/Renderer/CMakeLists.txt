add_library(NESRenderer 

    "BG/Renderer/Interface.h"
    "BG/Renderer/Interface.cpp"
    "BG/Renderer/Manager.h"
    "BG/Renderer/Manager.cpp"

)

target_link_libraries(NESRenderer

    bg-common-logger

    # vk-bootstrap::vk-bootstrap
    Vulkan::Vulkan
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    # glm::glm
    # unofficial::shaderc::shaderc
    # GPUOpen::VulkanMemoryAllocator
)
target_include_directories(NESRenderer PUBLIC ${SRC_DIR}/Renderer)