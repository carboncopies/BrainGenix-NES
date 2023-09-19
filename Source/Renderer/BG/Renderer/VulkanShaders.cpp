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


    // Now a bunch of boilerplate to finish setting up the vulkan graphics pipeline. This will probably need to be filled in more later
    VkPipelineVertexInputStateCreateInfo VertexInputInfo = {};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputInfo.vertexBindingDescriptionCount = 0;
    VertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo InputAssembly = {};
    InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    InputAssembly.primitiveRestartEnable = VK_FALSE;


    // Setup Viewport
    VkViewport Viewport = {};
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.width = (float)_RD->Optional_Swapchain_.extent.width;
    Viewport.height = (float)_RD->Optional_Swapchain_.extent.height;
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;

    VkRect2D Scissor = {};
    Scissor.offset = {0,0};
    Scissor.extent = _RD->Optional_Swapchain_.extent;


    VkPipelineViewportStateCreateInfo ViewportState = {};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.viewportCount = 1;
    ViewportState.pViewports = &Viewport;
    ViewportState.scissorCount = 1;
    ViewportState.pScissors = &Scissor;



    VkPipelineRasterizationStateCreateInfo Rasterizer = {};
    Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Rasterizer.depthClampEnable = VK_FALSE;
    Rasterizer.rasterizerDiscardEnable = VK_FALSE;
    Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    Rasterizer.lineWidth = 1.0f;
    Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    Rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    Rasterizer.depthBiasEnable = VK_FALSE;


    // Antialiasing stuff
    VkPipelineMultisampleStateCreateInfo Multisampling = {};
    Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Multisampling.sampleShadingEnable = VK_FALSE;
    Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;



    VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
    ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    ColorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo ColorBlending = {};
    ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	ColorBlending.logicOpEnable = VK_FALSE;
	ColorBlending.logicOp = VK_LOGIC_OP_COPY;
	ColorBlending.attachmentCount = 1;
	ColorBlending.pAttachments = &ColorBlendAttachment;
	ColorBlending.blendConstants[0] = 0.0f;
	ColorBlending.blendConstants[1] = 0.0f;
	ColorBlending.blendConstants[2] = 0.0f;
	ColorBlending.blendConstants[3] = 0.0f;


    // Finally, Create The Pipeline
    _Logger->Log("Setting Up Vulkan Pipeline Layout", 1);
    VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = 0;
    PipelineLayoutInfo.pushConstantRangeCount = 0;

    if (!vkCreatePipelineLayout(_RD->VulkanDevice_.device, &PipelineLayoutInfo, nullptr, &_RD->VulkanPipelineLayout_)) {
        _Logger->Log("Failed To Create Vulkan Pipeline Layout", 9);
        return false;
        
    }


    _Logger->Log("Creating Vulkan Pipeline", 2);
    std::vector<VkDynamicState> DynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    
    VkPipelineDynamicStateCreateInfo DynamicInfo = {};
    DynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    DynamicInfo.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size());
    DynamicInfo.pDynamicStates = DynamicStates.data();

    VkGraphicsPipelineCreateInfo PipelineInfo = {};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineInfo.stageCount = 2;
	PipelineInfo.pStages = ShaderStages;
	PipelineInfo.pVertexInputState = &VertexInputInfo;
	PipelineInfo.pInputAssemblyState = &InputAssembly;
	PipelineInfo.pViewportState = &ViewportState;
	PipelineInfo.pRasterizationState = &Rasterizer;
	PipelineInfo.pMultisampleState = &Multisampling;
	PipelineInfo.pColorBlendState = &ColorBlending;
	PipelineInfo.pDynamicState = &DynamicInfo;
	PipelineInfo.layout = _RD->VulkanPipelineLayout_;
	PipelineInfo.renderPass = _RD->VulkanRenderPass_;
	PipelineInfo.subpass = 0;
	PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (!vkCreateGraphicsPipelines(_RD->VulkanDevice_.device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &_RD->VulkanGraphicsPipeline)) {
        _Logger->Log("Failed To Create Vulkan Graphics Pipeline", 9);
        return false;
        
    }


    // Now, Cleanup The Old Shader Modules
    vkDestroyShaderModule(_RD->VulkanDevice_.device, VertexShaderModule, nullptr);
    vkDestroyShaderModule(_RD->VulkanDevice_.device, FragmentShaderModule, nullptr);

    return true;
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG