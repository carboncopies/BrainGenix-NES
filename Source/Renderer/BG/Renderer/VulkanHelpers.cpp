#include <BG/Renderer/VulkanHelpers.h>

// Todo: make the vulkan memory allocator use it's own class - probably have a list of allocations in the render data struct?
// Then, have a memory management class do the rest
// then, try and clean up this file

namespace BG {
namespace NES {
namespace Renderer {


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};



bool VulkanInit_CreateInstance(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    // Configure Vulkan Builder Helper Instance
    _Logger->Log("Preparing To Create Vulkan Instance", 3);
    
    vkb::InstanceBuilder VulkanInstanceBuilder;
    VulkanInstanceBuilder.set_app_name("BrainGenix-NES Neuron Emulation System");
    VulkanInstanceBuilder.set_engine_name("BrainGenix-NES Rendering Engine");
    VulkanInstanceBuilder.require_api_version(1, 0, 0);


    _Logger->Log("Querying Information About Available Vulkan Layers", 3);
    static vkb::Result<vkb::SystemInfo> SystemInfoReturn = vkb::SystemInfo::get_system_info();
    if (!SystemInfoReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed To Get Vulkan System Information Data", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + SystemInfoReturn.error().message();
        _Logger->Log(ErrorMessage, 10);
        
        return false;
    }

    vkb::SystemInfo SystemInfo = SystemInfoReturn.value();

    if (_RD->IsDebugging_) {
        _Logger->Log("Vulkan Validation Layers Have Been [REQUESTED]", 2);

        if (SystemInfo.is_layer_available("VK_LAYER_LUNARG_api_dump")) {
            _Logger->Log("Enabling Vulkan Layer 'VK_LAYER_LUNARG_api_dump'", 1);
            VulkanInstanceBuilder.enable_layer("VK_LAYER_LUNARG_api_dump");
        } else {
            _Logger->Log("Unable To Enable Vulkan Layer 'VK_LAYER_LUNARG_api_dump'", 7);
        }

        if (SystemInfo.validation_layers_available) {
            _Logger->Log("Enabling Vulkan Validation Layers", 1);
            VulkanInstanceBuilder.enable_validation_layers();
        } else {
            _Logger->Log("Unable To Enable Vulkan Validation Layers, As They Are Not Available (Check If They Have Been Installed?)", 8);
        }
    } else {
        _Logger->Log("Vulkan Validation Layers Have Been [DISABLED]", 2);
    }


    //FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
    /*
    FUTURE TODO: ADD OUR OWN VULKAN DEBUG MESSAGE CALLBACK HANDLER 
    (ALLOWS US TO LOG VULKAN ERRORS USING THE BG LOGGER, NOT VIA std::cout/std::cerr)

    Something like this:

        instance_builder.set_debug_callback (
        [] (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void *pUserData) 
            -> VkBool32 {
                auto severity = vkb::to_string_message_severity (messageSeverity);
                auto type = vkb::to_string_message_type (messageType);
                printf ("[%s: %s] %s\n", severity, type, pCallbackData->pMessage);
                return VK_FALSE;
            }
        );
    
    */
    _Logger->Log("FIXME: Add Our Own Vulkan Debug Callback System [Renderer]", 8);
    VulkanInstanceBuilder.use_default_debug_messenger(); // FIXME, SEE ABOVE FIXME NOTICE!
    //FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
    // Was that enough FIXMEs?
    // I think we need more.



    // Now, Create The Vulkan Instance
    _Logger->Log("Building Vulkan Instance", 4);
    vkb::Result<vkb::Instance> VulkanInstanceBuilderReturn = VulkanInstanceBuilder.build();
    if (!VulkanInstanceBuilderReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed To Create Vulkan Instance (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanInstanceBuilderReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }

    _RD->VulkanInstance_ = VulkanInstanceBuilderReturn.value();


    return true;
}

bool VulkanInit_Optional_CreateWindow(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    _Logger->Log("Debug Window Requested, Initializing SDL", 3);
    SDL_Init(SDL_INIT_VIDEO);

    _Logger->Log("Creating SDL Window", 3);
    SDL_WindowFlags WindowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
    _RD->Optional_SDLWindow_ = SDL_CreateWindow(
        "BrainGenix-NES Debugging Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        WindowFlags
    );
    if (_RD->Optional_SDLWindow_ == nullptr) {
        _Logger->Log("Failed to Create SDL Window (See Following Line For Error)", 10);
        std::string ErrorMessage = "SDL Reported Error: ";
        ErrorMessage += SDL_GetError();
        _Logger->Log(ErrorMessage, 10);
        return false;
    }

    _Logger->Log("Enabling Vulkan Extensions For Debug Window", 4);
    SDL_bool SurfaceCreateStatus = SDL_Vulkan_CreateSurface(_RD->Optional_SDLWindow_, _RD->VulkanInstance_.instance, &_RD->Optional_WindowSurface_);
    if (!SurfaceCreateStatus) {
        _Logger->Log("Failed to Create Vulkan Surface", 10);
        return false;
    }

    return true;
}

bool VulkanInit_CreateDevice(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);
   
    _Logger->Log("Selecting Physical Vulkan Capable Device From Available Devices", 3);
    vkb::PhysicalDeviceSelector VulkanPhysicalDeviceSelector(_RD->VulkanInstance_);

    // Prefer to pick a physical gpu rather than integrated or virtualized
    VulkanPhysicalDeviceSelector.prefer_gpu_device_type(vkb::PreferredDeviceType::discrete);
    VulkanPhysicalDeviceSelector.require_separate_transfer_queue();
    VulkanPhysicalDeviceSelector.require_separate_compute_queue();
    
    
    /*
        A quick note about vk-bootstrap and headless/offscreen rendering.
        It seems that by default it does not support this using the PhysicalDeviceSelector, however, there is an option to enable it.
        This is done via the (somewhat confusingly named) 'defer_surface_initialization'. 
        Please see the following if statement, which is what indicates this is to be disabled (if the debug window is NOT enabled).
    */
    if (!_RD->IsWindowed_) {
        _Logger->Log("System Is Running Headless, No Presentation Surface Will Be Used", 1);
        VulkanPhysicalDeviceSelector.defer_surface_initialization();
    } else {
        _Logger->Log("System Is Windowed, Setting Presentation Surface", 1);
        VulkanPhysicalDeviceSelector.set_surface(_RD->Optional_WindowSurface_);
    }

    // Note that if we need any extra extensions/requirements, this is the place to tell the selector to use them.

    vkb::Result<vkb::PhysicalDevice> VulkanPhysicalDeviceSelectorReturn = VulkanPhysicalDeviceSelector.select();;
    if (!VulkanPhysicalDeviceSelectorReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed To Select Vulkan Device (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanPhysicalDeviceSelectorReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    vkb::PhysicalDevice VulkanPhysicalDevice = VulkanPhysicalDeviceSelectorReturn.value();
    _Logger->Log(std::string("Selected Physical Device: ") + VulkanPhysicalDevice.name, 4);




    // Now, We Create The Logical Device (VkDevice) Instance On The Selected Physical Device
    _Logger->Log("Creating Logical Device On Selected Physical Device", 4);
    vkb::DeviceBuilder VulkanDeviceBuilder{VulkanPhysicalDevice};
    vkb::Result<vkb::Device> VulkanDeviceBuilderReturn = VulkanDeviceBuilder.build();
    if (!VulkanDeviceBuilderReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed To Create Logical Vulkan Device (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanDeviceBuilderReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    _RD->VulkanDevice_ = VulkanDeviceBuilderReturn.value();



    // // Setup Memory Allocator Instance
    // VmaVulkanFunctions VulkanFunctions = {};
    // VulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    // VulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
    
    // VmaAllocatorCreateInfo AllocatorCreateInfo = {};
    // AllocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    // AllocatorCreateInfo.physicalDevice = _RD->VulkanDevice_.physical_device;
    // AllocatorCreateInfo.device = _RD->VulkanDevice_.device;
    // AllocatorCreateInfo.instance = _RD->VulkanInstance_.instance;
    // // AllocatorCreateInfo.pVulkanFunctions = &_RD->VulkanInstance_.;
    
    // vmaCreateAllocator(&AllocatorCreateInfo, &_RD->Allocator_);

    return true;
}

bool VulkanInit_CreateQueues(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    // Create Vulkan Queues
    _Logger->Log("Setting Up Vulkan Graphics Queue", 4);
    vkb::Result<VkQueue> VulkanGraphicsQueueReturn = _RD->VulkanDevice_.get_queue(vkb::QueueType::graphics);
    if (!VulkanGraphicsQueueReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed to Get Vulkan Graphics Queue (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanGraphicsQueueReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    _RD->VulkanGraphicsQeueue_ = VulkanGraphicsQueueReturn.value();

    _Logger->Log("Setting Up Vulkan Transfer Queue", 4);
    vkb::Result<VkQueue> VulkanTransferQueueReturn = _RD->VulkanDevice_.get_queue(vkb::QueueType::transfer);
    if (!VulkanTransferQueueReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed to Get Vulkan Transfer Queue (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanTransferQueueReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    _RD->VulkanTransferQeueue_ = VulkanTransferQueueReturn.value();

    _Logger->Log("Setting Up Vulkan Present Queue", 4);
    vkb::Result<VkQueue> VulkanPresentQueueReturn = _RD->VulkanDevice_.get_queue(vkb::QueueType::present);
    if (!VulkanPresentQueueReturn) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed to Get Vulkan Present Queue (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + VulkanPresentQueueReturn.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    _RD->VulkanPresentQueue_ = VulkanPresentQueueReturn.value();

    return true;

}

bool VulkanInit_Optional_CreateSwapchain(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    _Logger->Log("Setting Up Vulkan Swapchain For Windowed Rendering", 4);
    vkb::SwapchainBuilder VulkanSwapchainBuilder{_RD->VulkanDevice_};
    vkb::Result<vkb::Swapchain> SwapchainResult = VulkanSwapchainBuilder.build();
    if (!SwapchainResult) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed to Create Vulkan Swapchain (See Following Line For Error)", 10);
        std::string ErrorMessage = "VK_Bootstrap Reported Error: " + SwapchainResult.error().message();
        _Logger->Log(ErrorMessage, 10);

        return false;
    }
    _RD->Optional_Swapchain_ = SwapchainResult.value();


    // Update RenderData ColorFormat Enum
    _RD->VulkanColorFormat_ = _RD->Optional_Swapchain_.image_format;


    return true;
}

bool VulkanInit_CreateRenderPass(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);


    // FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME 
    // This will likely need to be updated for headless rendering - something about a framebuffer and more stuff. 
    // I'm leaving it alone for now since I'm not sure how to implement that stuff yet. 
    // FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME 
    _Logger->Log("FIXME: SETUP HEADLESS RENDERPASS FRAMEBUFFER STUFF OR SOMETHING (see example_headless.cpp)", 8);


    // Create Color ATtachment for Renderpass
    VkAttachmentDescription ColorAttachment{};
    ColorAttachment.format = _RD->VulkanColorFormat_;
	ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    VkAttachmentReference ColorAttachmentRef{};
	ColorAttachmentRef.attachment = 0;
	ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription Subpass{};
	Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	Subpass.colorAttachmentCount = 1;
	Subpass.pColorAttachments = &ColorAttachmentRef;

	VkSubpassDependency Dependency{};
	Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	Dependency.dstSubpass = 0;
	Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	Dependency.srcAccessMask = 0;
	Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT

	VkRenderPassCreateInfo RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassInfo.attachmentCount = 1;
	RenderPassInfo.pAttachments = &ColorAttachment;
	RenderPassInfo.subpassCount = 1;
	RenderPassInfo.pSubpasses = &Subpass;
	RenderPassInfo.dependencyCount = 1;
	RenderPassInfo.pDependencies = &Dependency;

    // So apparently, VkResult cannot just be checked in an if statement - doing if (!Status) WILL NOT WORK
    // This cost me so much time ~13 hours. WHY KHRONOS, WHY!
    // So, I highly suggest not doing that. Instead, do if (Status != VK_SUCCESS) which will actually work, unlike the other thing.
    // Rant over.
    _Logger->Log("Creating Vulkan Render Pass", 2);
    VkResult Status = vkCreateRenderPass(_RD->VulkanDevice_.device, &RenderPassInfo, nullptr, &_RD->VulkanRenderPass_);
    if (Status != VK_SUCCESS) {
        _Logger->Log("Failed To Create Vulkan Render Pass", 9);
        _Logger->Log(std::string("vkCreateRenderPass Returned Code ") + std::to_string(Status), 9);
        
        return false;
    }


    return true;

}

bool VulkanInit_CreateGraphicsPipeline(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);


    _Logger->Log("Creating Graphics Pipeline", 3);


    // Load Shader Strings
    _Logger->Log("Loading GLSL Shader Code"), 3;
    std::string VertShader = Internal::ReadFileToString("test.vert");
    std::string FragShader = Internal::ReadFileToString("test.frag");

    // Now, Compile Into Bytecode
    _Logger->Log("Compiling Shaders Into SPIR-V Bytecode", 2);
    std::vector<uint32_t> VertBytecode;
    std::vector<uint32_t> FragBytecode;
    Internal::Vulkan_DynamicallyCompileShader(_Logger, VertShader, "test.vert", shaderc_glsl_vertex_shader,   &VertBytecode, false, true);
    Internal::Vulkan_DynamicallyCompileShader(_Logger, FragShader, "test.frag", shaderc_glsl_fragment_shader, &FragBytecode, false, true);


    // Build Shader Pipelines
    _Logger->Log("Building Shader Modules From SPIR-V Bytecode", 2);
    VkShaderModule VertexShaderModule;
    VkShaderModule FragmentShaderModule;
    if (!Internal::Vulkan_CreateShaderModule(_Logger, _RD, &VertBytecode, &VertexShaderModule)) {
        return false;
    }
    if (!Internal::Vulkan_CreateShaderModule(_Logger, _RD, &FragBytecode, &FragmentShaderModule)) {
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
    auto BindingDescription = Vertex::getBindingDescription();
    auto AttributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo VertexInputInfo = {};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VertexInputInfo.vertexBindingDescriptionCount = 1;
    VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(AttributeDescriptions.size());
    VertexInputInfo.pVertexBindingDescriptions = &BindingDescription;
    VertexInputInfo.pVertexAttributeDescriptions = AttributeDescriptions.data();



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

    VkResult PipelineLayoutCreateStatus = vkCreatePipelineLayout(_RD->VulkanDevice_.device, &PipelineLayoutInfo, nullptr, &_RD->VulkanPipelineLayout_);
    if (PipelineLayoutCreateStatus != VK_SUCCESS) {
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

    VkResult PipelineCreateStatus = vkCreateGraphicsPipelines(_RD->VulkanDevice_.device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &_RD->VulkanGraphicsPipeline_);
    if (PipelineCreateStatus != VK_SUCCESS) {
        _Logger->Log("Failed To Create Vulkan Graphics Pipeline", 9);
        return false;
        
    }


    // Now, Cleanup The Old Shader Modules
    vkDestroyShaderModule(_RD->VulkanDevice_.device, VertexShaderModule, nullptr);
    vkDestroyShaderModule(_RD->VulkanDevice_.device, FragmentShaderModule, nullptr);

    return true;
}

bool VulkanInit_CreateFramebuffer(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);


    // FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME 
    // This section will also need to be updated for headless rendering!
    // We probably will need to make the framebuffer in a different way for headless rendering, since the swapchain doesn't exist without a window.
    // FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME 
    _Logger->Log("FIXME: Update CreateFramebuffer For Headless Rendering", 8);
    


    _Logger->Log("Creating Vulkan Framebuffers", 2);
    _RD->VulkanSwapchainImages_ = _RD->Optional_Swapchain_.get_images().value();
    _RD->VulkanSwapchainImageViews_ = _RD->Optional_Swapchain_.get_image_views().value();

    _RD->VulkanFramebuffers_.resize(_RD->VulkanSwapchainImageViews_.size());
    for (int i = 0; i < _RD->VulkanSwapchainImageViews_.size(); i++) {
        VkImageView Attachments[] = {_RD->VulkanSwapchainImageViews_[i]};

        VkFramebufferCreateInfo FramebufferCreateInfo{};
        FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        FramebufferCreateInfo.renderPass = _RD->VulkanRenderPass_;
        FramebufferCreateInfo.attachmentCount = 1;
        FramebufferCreateInfo.pAttachments = Attachments;
        FramebufferCreateInfo.width = _RD->Width_;
        FramebufferCreateInfo.height = _RD->Height_;
        FramebufferCreateInfo.layers = 1;

        VkResult FramebufferCreateStatus = vkCreateFramebuffer(_RD->VulkanDevice_.device, &FramebufferCreateInfo, nullptr, &_RD->VulkanFramebuffers_[i]);
        if (FramebufferCreateStatus != VK_SUCCESS) {
            _Logger->Log("Error During Renderer Initialization", 10);
            _Logger->Log("Failed To Create Vulkan Framebuffer (vkCreateFramebuffer returned !VK_SUCCESS)", 10);
            return false;
        }

    }

    return true;

}

bool VulkanInit_CreateCommandPool(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    _Logger->Log("Creating Vulkan Command Pool", 2);
    VkCommandPoolCreateInfo PoolCreateInfo{};
    PoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    PoolCreateInfo.queueFamilyIndex = _RD->VulkanDevice_.get_queue_index(vkb::QueueType::graphics).value();

    VkResult CommandPoolCreateStatus = vkCreateCommandPool(_RD->VulkanDevice_.device, &PoolCreateInfo, nullptr, &_RD->VulkanCommandPool_);
    if (CommandPoolCreateStatus != VK_SUCCESS) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed To Create Vulkan Command Pool (vkCreateCommandPool returned !VK_SUCCESS)", 10);
        return false;
    }

    return true;
}

bool VulkanInit_CreateCommandBuffers(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    _Logger->Log("Creating Vulkan Command Buffers", 2);
    //FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
    _Logger->Log("FIXME: UPDATE COMMAND BUFFER CREATE FUNC WITH HEADLESS COMMAND BUFFER INFO!", 8);
    //FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
    _RD->VulkanCommandBuffers_.resize(_RD->VulkanFramebuffers_.size());

    VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
    CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    CommandBufferAllocateInfo.commandPool = _RD->VulkanCommandPool_;
    CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    CommandBufferAllocateInfo.commandBufferCount = (uint32_t)_RD->VulkanCommandBuffers_.size();

    VkResult CommandBufferCreateStatus = vkAllocateCommandBuffers(_RD->VulkanDevice_.device, &CommandBufferAllocateInfo, _RD->VulkanCommandBuffers_.data());
    if (CommandBufferCreateStatus != VK_SUCCESS) {
        _Logger->Log("Error During Renderer Initialization", 10);
        _Logger->Log("Failed To Allocate Vulkan Command Buffers (vkAllocateCommandBuffers returned !VK_SUCCESS)", 10);
        return false;
    }


    // Populate The Command Buffers
    for (unsigned int i = 0; i < _RD->VulkanCommandBuffers_.size(); i++) {

        VkCommandBufferBeginInfo BeginInfo{};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        
        VkResult BeginCommandBufferResult = vkBeginCommandBuffer(_RD->VulkanCommandBuffers_[i], &BeginInfo);
        if (BeginCommandBufferResult != VK_SUCCESS) {
            _Logger->Log("Failed To Begin Vulkan Command Buffer", 10);
            return false;
        }

        VkClearValue ClearColor = {{{0., 0., 0., 1.}}};

        VkRenderPassBeginInfo RenderPassBeginInfo{};
        RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassBeginInfo.renderPass = _RD->VulkanRenderPass_;
        RenderPassBeginInfo.framebuffer = _RD->VulkanFramebuffers_[i];
        RenderPassBeginInfo.renderArea.offset = {0,0};
        RenderPassBeginInfo.renderArea.extent = _RD->Optional_Swapchain_.extent;
        RenderPassBeginInfo.clearValueCount = 1;
        RenderPassBeginInfo.pClearValues = &ClearColor;


        VkViewport Viewport{};
        Viewport.x = 0.;
        Viewport.y = 0.;
        Viewport.width = _RD->Width_;
        Viewport.height = _RD->Height_;
        Viewport.minDepth = 0.;
        Viewport.maxDepth = 1.;
        
        VkRect2D Scissor = {};
        Scissor.offset = {0,0};
        Scissor.extent = _RD->Optional_Swapchain_.extent;


        // Now Record Into The Command Buffer
        vkCmdSetViewport(_RD->VulkanCommandBuffers_[i], 0, 1, &Viewport);
        vkCmdSetScissor(_RD->VulkanCommandBuffers_[i], 0, 1, &Scissor);
        
        vkCmdBeginRenderPass(_RD->VulkanCommandBuffers_[i], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(_RD->VulkanCommandBuffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _RD->VulkanGraphicsPipeline_);

        vkCmdDraw(_RD->VulkanCommandBuffers_[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(_RD->VulkanCommandBuffers_[i]);

        VkResult EndCommandBufferResult = vkEndCommandBuffer(_RD->VulkanCommandBuffers_[i]);
        if (EndCommandBufferResult != VK_SUCCESS) {
            _Logger->Log("Failed To End Vulkan Command Buffer", 10);
            return false;
        }

    }

    return true;
}

bool VulkanInit_CreateVertexBuffer(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    _Logger->Log("Creating Vulkan Vertex Buffer", 2);
    // VkBufferCreateInfo BufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    // BufferInfo.size = 65536;
    // BufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    // VmaAllocationCreateInfo AllocInfo = {};
    // AllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

    // VkBuffer Buffer;
    // VmaAllocation Allocation;
    // vmaCreateBuffer(_RD->Allocator_, &BufferInfo, &AllocInfo, &Buffer, &Allocation, nullptr);

    // probably should convert this into a class that manages the allocations (perhaps so we can just have it manage everything for us.)

    // return true;

}

bool VulkanInit_CreateSyncObjects(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    
    _Logger->Log("Creating Vulkan Synchronization Objects", 2);
    _RD->VulkanAvailableSemaphores_.resize(_RD->MaxFramesInTransit_);
    _RD->VulkanFinishedSemaphores_.resize(_RD->MaxFramesInTransit_);
    _RD->VulkanInTransitFences_.resize(_RD->MaxFramesInTransit_);
    _RD->VulkanImageInTransit_.resize(_RD->Optional_Swapchain_.image_count, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo SemaphoreCreateInfo{};
    SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo FenceCreateInfo{};
    FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (unsigned int i = 0; i < _RD->MaxFramesInTransit_; i++) {

        // Create Available Semaphores
        VkResult SemaphoreCreateStatus = vkCreateSemaphore(_RD->VulkanDevice_.device, &SemaphoreCreateInfo, nullptr, &_RD->VulkanAvailableSemaphores_[i]);
        if (SemaphoreCreateStatus != VK_SUCCESS) {
            _Logger->Log("Error During Renderer Initialization", 10);
            _Logger->Log("Failed To Create Vulkan Semaphore (vkCreateSemaphore returned !VK_SUCCESS)", 10);
            return false;
        }

        // Create Finished Semaphores
        SemaphoreCreateStatus = vkCreateSemaphore(_RD->VulkanDevice_.device, &SemaphoreCreateInfo, nullptr, &_RD->VulkanFinishedSemaphores_[i]);
        if (SemaphoreCreateStatus != VK_SUCCESS) {
            _Logger->Log("Error During Renderer Initialization", 10);
            _Logger->Log("Failed To Create Vulkan Semaphore (vkCreateSemaphore returned !VK_SUCCESS)", 10);
            return false;
        }

        // Create Fences
        VkResult FenceCreateStatus = vkCreateFence(_RD->VulkanDevice_.device, &FenceCreateInfo, nullptr, &_RD->VulkanInTransitFences_[i]);
        if (FenceCreateStatus != VK_SUCCESS) {
            _Logger->Log("Error During Renderer Initialization", 10);
            _Logger->Log("Failed To Create Vulkan Fence (vkCreateFence returned !VK_SUCCESS)", 10);
            return false;
        }

    }

    return true;

}


bool VulkanUtil_RecreateSwapchain(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);


    // Wait Until GPU Is Done Doing Stuff
    vkDeviceWaitIdle(_RD->VulkanDevice_.device);


    // Cleanup Existing VK Objects
    vkDestroyCommandPool(_RD->VulkanDevice_.device, _RD->VulkanCommandPool_, nullptr);

    for (unsigned int i = 0; i < _RD->VulkanFramebuffers_.size(); i++) {
        vkDestroyFramebuffer(_RD->VulkanDevice_.device, _RD->VulkanFramebuffers_[i], nullptr);
    } 

    _RD->Optional_Swapchain_.destroy_image_views(_RD->VulkanSwapchainImageViews_);

    
    // Now, recreate them at the given resolution
    if (!VulkanInit_Optional_CreateSwapchain(_Logger, _RD)) {
        return false;
    }
    if (!VulkanInit_CreateFramebuffer(_Logger, _RD)) {
        return false;
    }
    if (!VulkanInit_CreateCommandPool(_Logger, _RD)) {
        return false;
    }
    if (!VulkanInit_CreateCommandBuffers(_Logger, _RD)) {
        return false;
    }

    return true;
    
}

bool VulkanUtil_WaitUntilGPUDone(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr && "_Logger pointer cannot be null");
    assert(_RD != nullptr && "_RD pointer cannot be null");

    for (unsigned int i = 0; i < _RD->VulkanInTransitFences_.size(); i++) {
        vkWaitForFences(_RD->VulkanDevice_.device, 1, &_RD->VulkanInTransitFences_[i], VK_TRUE, UINT64_MAX);
    }

    return true;

}


bool VulkanDeinit_DestroyAll(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);

    // Cleanup Windowed (Optional) Vulkan Objects
    if (_RD->Optional_Swapchain_) {
        _Logger->Log("Cleaning Up Optional Swapchain Surface", 2);
        _RD->Optional_Swapchain_.destroy_image_views(_RD->VulkanSwapchainImageViews_);
        vkb::destroy_swapchain(_RD->Optional_Swapchain_);
    }

    if (_RD->Optional_WindowSurface_) {
        _Logger->Log("Cleaning Up Optional Window Surface", 2);
        vkb::destroy_surface(_RD->VulkanInstance_, _RD->Optional_WindowSurface_);
    }


    // Shutdown SDL (If Enabled)
    if (_RD->IsWindowed_) {
        SDL_Quit();
    }


    // Cleanup Required Vulkan Objects
    _Logger->Log("Cleaning Up Vulkan Sync Objects", 2);
    for (unsigned int i = 0; i < _RD->VulkanFinishedSemaphores_.size(); i++) {
        vkDestroySemaphore(_RD->VulkanDevice_.device, _RD->VulkanFinishedSemaphores_[i], nullptr);
    }
    for (unsigned int i = 0; i < _RD->VulkanAvailableSemaphores_.size(); i++) {
        vkDestroySemaphore(_RD->VulkanDevice_.device, _RD->VulkanAvailableSemaphores_[i], nullptr);
    }
    for (unsigned int i = 0; i < _RD->VulkanInTransitFences_.size(); i++) {
        vkDestroyFence(_RD->VulkanDevice_.device, _RD->VulkanInTransitFences_[i], nullptr);
    }

    _Logger->Log("Cleaning Up Vulkan Command Pool", 2);
    if (_RD->VulkanCommandPool_) {
        vkDestroyCommandPool(_RD->VulkanDevice_.device, _RD->VulkanCommandPool_, nullptr);
    }

    _Logger->Log("Cleaning Up Vulkan Pipeline", 2);
    if (_RD->VulkanGraphicsPipeline_) {
        vkDestroyPipeline(_RD->VulkanDevice_.device, _RD->VulkanGraphicsPipeline_, nullptr);
    }

    _Logger->Log("Cleaning Up Vulkan Pipeline Layout", 2);
    if (_RD->VulkanPipelineLayout_) {
        vkDestroyPipelineLayout(_RD->VulkanDevice_.device, _RD->VulkanPipelineLayout_, nullptr);
    }

    _Logger->Log("Cleaning Up Vulkan Renderpass", 2);
    if (_RD->VulkanRenderPass_) {
        vkDestroyRenderPass(_RD->VulkanDevice_.device, _RD->VulkanRenderPass_, nullptr);
    }

    _Logger->Log("Cleaning Up Vulkan Framebuffers", 2);
    for (unsigned int i = 0; i < _RD->VulkanFramebuffers_.size(); i++) {
        vkDestroyFramebuffer(_RD->VulkanDevice_.device, _RD->VulkanFramebuffers_[i], nullptr);
    }

    _Logger->Log("Cleaning Up VMA Allocator", 2);
    if (_RD->Allocator_) {
        vmaDestroyAllocator(_RD->Allocator_);
    }

    _Logger->Log("Cleaning Up Vulkan Logical Device", 2);
    if (_RD->VulkanDevice_) {
        vkb::destroy_device(_RD->VulkanDevice_);
    }

    _Logger->Log("Cleaning Up Vulkan Instance", 2);
    if (_RD->VulkanInstance_) {
        vkb::destroy_instance(_RD->VulkanInstance_);
    }

    return true;

}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG