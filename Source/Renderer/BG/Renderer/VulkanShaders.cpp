#include <BG/Renderer/VulkanShaders.h>



namespace BG {
namespace NES {
namespace Renderer {
namespace Internal {

// copy pasted from vulkan-tutorial's shader_module page. Temporary, until we implement dynamic shader compilation.
std::vector<char> ReadFile(std::string filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;

}


bool Vulkan_CreateShaderModule(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD, const std::vector<char>* _ShaderBytecode, VkShaderModule* _ShaderModule) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);
    assert(_ShaderBytecode != nullptr);
    assert(_ShaderModule != nullptr);

    // Setup Shader Create Info
    VkShaderModuleCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    CreateInfo.codeSize = _ShaderBytecode->size();
    CreateInfo.pCode = reinterpret_cast<const uint32_t*>(_ShaderBytecode->data());

    // Now Setup The Module
    if (!vkCreateShaderModule(_RD->VulkanDevice_.device, &CreateInfo, nullptr, _ShaderModule)) {
        _Logger->Log("Error During Shader Module Creation, Failed To Create Shader Module", 8);
        return false;
    }
    return true;
}

}; // Close Namespace Internal

bool Vulkan_CreateGraphicsPipeline(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);


    _Logger->Log("Creating Graphics Pipeline", 3);


    // Load Shaders - TEMPORARY FIXME LATER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    _Logger->Log("Using [FIXME] SPIRV Loader To Load Precompiled Shaders ", 7);
    auto VertShaderCode = Internal::ReadFile("vert.spv");
    auto FragShaderCode = Internal::ReadFile("frag.spv");


    // TODO: ADD SHADER COMPILATION STAGE HERE - WE WANT TO DO DYNAMIC SHADER COMPILATION (AND MAYBE ALSO IMPLEMENT SHADER CACHING)
    _Logger->Log("FIXME: ADD SHADER COMPILATION STAGE HERE!", 9);


    // Build Shader Pipelines
    _Logger->Log("Building Shader Modules From SPIR-V Bytecode", 2);
    VkShaderModule VertexShaderModule;
    VkShaderModule FragmentShaderModule;
    if (!Internal::Vulkan_CreateShaderModule(_Logger, _RD, &VertShaderCode, &VertexShaderModule)) {
        return false;
    }
    if (!Internal::Vulkan_CreateShaderModule(_Logger, _RD, &FragShaderCode, &FragmentShaderModule)) {
        return false;
    }


    // Build Shader Pipeline Stages
    VkPipelineShaderStageCreateInfo VertShaderStageInfo{};
    VertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    VertShaderStageInfo.module = VertexShaderModule;
    VertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo FragShaderStageInfo{};
    FragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    FragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    FragShaderStageInfo.module = FragmentShaderModule;
    FragShaderStageInfo.pName = "main";


    VkPipelineShaderStageCreateInfo ShaderStages[] = {VertShaderStageInfo, FragShaderStageInfo};



    // Now, Cleanup The Old Shader Modules
    vkDestroyShaderModule(_RD->VulkanDevice_.device, VertexShaderModule, nullptr);
    vkDestroyShaderModule(_RD->VulkanDevice_.device, FragmentShaderModule, nullptr);
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG