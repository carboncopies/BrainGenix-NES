add_library(NESRenderer 

    "BG/Renderer/Interface.h"
    "BG/Renderer/Interface.cpp"
    "BG/Renderer/Manager.h"
    "BG/Renderer/Manager.cpp"
    "BG/Renderer/VulkanHelpers.h"
    "BG/Renderer/VulkanHelpers.cpp"

    "BG/Renderer/VulkanRenderer.h"
    "BG/Renderer/VulkanRenderer.cpp"
    "BG/Renderer/VulkanMemoryAllocator.h"
    "BG/Renderer/VulkanMemoryAllocator.cpp"
    "BG/Renderer/RenderData.h"
    "BG/Renderer/RenderData.cpp"
    "BG/Renderer/Allocation.h"
    "BG/Renderer/Allocation.cpp"

    "BG/Renderer/Shader/ShaderCompileObject.h"
    "BG/Renderer/Shader/ShaderCompileObject.cpp"
    "BG/Renderer/Shader/ShaderCompiler.h"
    "BG/Renderer/Shader/ShaderCompiler.cpp"
    "BG/Renderer/Shader/ShaderHelpers.h"
    "BG/Renderer/Shader/ShaderHelpers.cpp"
)

target_link_libraries(NESRenderer

    bg-common-logger

    vk-bootstrap::vk-bootstrap
    Vulkan::Vulkan
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    glm::glm
    unofficial::shaderc::shaderc
    GPUOpen::VulkanMemoryAllocator
)
target_include_directories(NESRenderer PUBLIC ${SRC_DIR}/Renderer)