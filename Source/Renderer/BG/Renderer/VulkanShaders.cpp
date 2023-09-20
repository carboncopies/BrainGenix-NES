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
    VkResult ShaderCreateStatus = vkCreateShaderModule(_RD->VulkanDevice_.device, &CreateInfo, nullptr, _ShaderModule);
    if (ShaderCreateStatus != VK_SUCCESS) {
        _Logger->Log("Error During Shader Module Creation, Failed To Create Shader Module", 8);
        return false;
    }
    return true;
}

}; // Close Namespace Internal


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG