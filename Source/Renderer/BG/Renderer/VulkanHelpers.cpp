#include <BG/Renderer/VulkanHelpers.h>



namespace BG {
namespace NES {
namespace Renderer {


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

    VkResult PipelineCreateStatus = vkCreateGraphicsPipelines(_RD->VulkanDevice_.device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &_RD->VulkanGraphicsPipeline);
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





}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG