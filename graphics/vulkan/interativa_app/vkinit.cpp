bool createPipelineLayout(VkDevice device,VkDescriptorSetLayout dsLayout,VkPipelineLayout* pipelineLayout){
    const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,.pNext = nullptr,.flags = 0,.setLayoutCount = 1,.pSetLayouts = &dsLayout,.pushConstantRangeCount =0,.pPushConstantRanges=nullptr};
    return vkCreatePipelineLayout(device,&pipelineLayoutInfo,nullptr,pipelineLayout) ==VK_SUCCESS;}
enum eRenderPassBit : uint8_t{
    eRenderPassBit_First = 0x01,              // clear the attachment
    eRenderPassBit_Last = 0x02,               // transition to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    eRenderPassBit_Offscreen = 0x04,          // transition to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    eRenderPassBit_OffscreenInternal = 0x08}; // keep VK_IMAGE_LAYOUT_*_ATTACHMENT_OPTIMAL
struct RenderPassCreateInfo final{
    bool clearColor_ = false;
    bool clearDepth_ = false;
    uint8_t flags_ = 0;};
bool createColorAndDepthRenderPass(VulkanRenderDevice& vkDev,bool useDepth,VkRenderPass* renderPass,const RenderPassCreateInfo& ci,VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM){
    const bool offscreenInt = ci.flags_ & eRenderPassBit_OffscreenInternal;
    const bool first = ci.flags_ & eRenderPassBit_First;
    const bool last =ci.flags_ & eRenderPassBit_Last;
    VkAttachmentDescription colorAttachment = {.flags=0,.format=colorFormat,.samples = VK_SAMPLE_COUNT_1_BIT,.loadOp = offscreenInt ? VK_ATTACHMENT_LOAD_OP_LOAD : (ci.clearColor_ ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD),.storeOp = VK_ATTACHMENT_STORE_OP_STORE,.stencilLoadOp=VK_ATTACHMENT_LOAD_OP_DONT_CARE,.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,.initialLayout = first?VK_IMAGE_LAYOUT_UNDEFINED : (offscreenInt ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),.finalLayout = last ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    const VkAttachmentReference colorAttachmentRef = {.attachment =0,.layout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkAttachmentDescription depthAttachment = {.flags = 0,.format=useDepth ? findDepthFormat(vkDev.physicalDevice) : VK_FORMAT_D32_SFLOAT,.samples = VK_SAMPLE_COUNT_1_BIT,.loadOp = offscreenInt ? VK_ATTACHMENT_LOAD_OP_LOAD : (ci.clearDepth_ ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD),.storeOp = VK_ATTACHMENT_STORE_OP_STORE,.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,.initialLayout=ci.clearDepth_? VK_IMAGE_LAYOUT_UNDEFINED : (offscreenInt ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
    const VkAttachmentReference depthAttachmentRef = {.attachment =1,.layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
    const VkSubpassDependency dependency = {.srcSubpass=VK_SUBPASS_EXTERNAL,.dstSubpass =0,.srcStageMask=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,.dstStageMask=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,.srcAccessMask=0,.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,.dependencyFlags = 0};
    std::vector<VkSubpassDependency> dependencies = {dependency};
    if (ci.flags_ & eRenderPassBit_Offscreen){
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        dependencies.resize(2);
        dependencies[0] = { .srcSubpass = VK_SUBPASS_EXTERNAL,.dstSubpass = 0,.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,.srcAccessMask = VK_ACCESS_SHADER_READ_BIT,.dstAccessMask=VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT};
        dependencies[1] = { .srcSubpass =0,.dstSubpass = VK_SUBPASS_EXTERNAL,.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,.dependencyFlags= VK_DEPENDENCY_BY_REGION_BIT};}
    const VkSubpassDescription subpass = {.flags = 0,.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,.inputAttachmentCount = 0,.pInputAttachments=nullptr,.colorAttachmentCount = 1,.pColorAttachments = &colorAttachmentRef,.pResolveAttachments = nullptr,.pDepthStencilAttachment = useDepth ? &depthAttachmentRef : nullptr,.preserveAttachmentCount = 0,.pPreserveAttachments =nullptr};
    std::array<VkAttachmentDescription,2> attachments = {colorAttachment,depthAttachment};
    const VkRenderPassCreateInfo renderPassInfo = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,.attachmentCount = static_cast<uint32_t>(useDepth? 2:1),.pAttachments=attachments.data(),.subpassCount=1,.pSubpasses=&subpass,.dependencyCount = static_cast<uint32_t>(dependencies.size()),.pDependencies=dependencies.data()};
    return (vkCreateRenderPass(vkDev.device,&renderPassInfo,nullptr,renderPass) == VK_SUCCESS);}
bool createColorAndDepthFramebuffer(VulkanRenderDevice& vkDev,uint32_t width,uint32_t height,VkRenderPass renderPass,VkImageView depthImageView,VkImageView swapchainImageView,VkFramebuffer* framebuffer){
    std::array<VkImageView,2> attachments = {swapchainImageView,depthImageView};
    const VkFramebufferCreateInfo fi = {.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,.pNext = nullptr,.flags = 0,.renderPass = renderPass,.attachmentCount = static_cast<uint32_t>(depthImageView == VK_NULL_HANDLE ? 1 : 2),.pAttachments = attachments.data(),.width = width,.height = height,.layers = 1};
    return vkCreateFramebuffer(vkDev.device,&fi,nullptr,framebuffer) == VK_SUCCESS;}
bool createColorAndDepthFramebuffers(VulkanRenderDevice& vkDev,VkRenderPass renderPass,VkImageView depthImageView,std::vector<VkFramebuffer>& swapchainFramebuffers){
    swapchainFramebuffers.resize(vkDev.swapchainImageViews.size());
    for(size_t i=0;i<vkDev.swapchainImageViews.size();i++)
        if(!createColorAndDepthFramebuffer(vkDev,vkDev.framebufferWidth,vkDev.framebufferHeight,renderPass,depthImageView,vkDev.swapchainImageViews[i],&swapchainFramebuffers[i]))
            return false;
    return true;}
static VkShaderStageFlagBits shaderStageFromFileName(const char* fileName){
    const std::string s(fileName);
    auto ends = [&s](const char* e){ const std::string t(e); return s.size() >= t.size() && s.compare(s.size()-t.size(),t.size(),t) == 0;};
    if(ends(".frag")) return VK_SHADER_STAGE_FRAGMENT_BIT;
    if(ends(".geom")) return VK_SHADER_STAGE_GEOMETRY_BIT;
    if(ends(".comp")) return VK_SHADER_STAGE_COMPUTE_BIT;
    if(ends(".tesc")) return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    if(ends(".tese")) return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    return VK_SHADER_STAGE_VERTEX_BIT;}
// replaces the old (device,width,height,...,shaderStages) version: takes shader file names, as the renderers call it
bool createGraphicsPipeline(VulkanRenderDevice& vkDev,VkRenderPass renderPass,VkPipelineLayout pipelineLayout,const std::vector<const char*>& shaderFiles,VkPipeline *pipeline,VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,bool useDepth = true,bool useBlending = false,bool dynamicScissorState = false){
    std::vector<ShaderModule> shaderModules(shaderFiles.size());
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages(shaderFiles.size());
    for(size_t i=0;i<shaderFiles.size();i++){
        VK_CHECK(createShaderModule(vkDev.device,&shaderModules[i],shaderFiles[i]));
        shaderStages[i] = VkPipelineShaderStageCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,.pNext = nullptr,.flags = 0,.stage = shaderStageFromFileName(shaderFiles[i]),.module = shaderModules[i].shaderModule,.pName = "main",.pSpecializationInfo = nullptr};}
    const uint32_t width = vkDev.framebufferWidth;
    const uint32_t height = vkDev.framebufferHeight;
    const VkPipelineVertexInputStateCreateInfo
        vertexInputInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    const VkPipelineInputAssemblyStateCreateInfo inputAssembly = {.sType=VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,.topology = topology,.primitiveRestartEnable = VK_FALSE};
    const VkViewport viewport = {.x = 0.0f,.y=0.0f,.width =static_cast<float>(width),.height=static_cast<float>(height),.minDepth=0.0f,.maxDepth=1.0f};
    const VkRect2D scissor = {.offset={0,0},.extent={width,height}};
    const VkPipelineViewportStateCreateInfo viewportState={.sType=VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,.viewportCount=1,.pViewports = &viewport,.scissorCount=1,.pScissors=&scissor};
    const VkPipelineRasterizationStateCreateInfo
        rasterizer = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,.polygonMode = VK_POLYGON_MODE_FILL,.cullMode=VK_CULL_MODE_NONE,.frontFace=VK_FRONT_FACE_CLOCKWISE,.lineWidth=1.0f};
    const VkPipelineMultisampleStateCreateInfo multisampling = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,.rasterizationSamples=VK_SAMPLE_COUNT_1_BIT,.sampleShadingEnable = VK_FALSE,.minSampleShading=1.0f};
    const VkPipelineColorBlendAttachmentState colorBlendAttachment = {.blendEnable = useBlending ? VK_TRUE : VK_FALSE,.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,.colorBlendOp = VK_BLEND_OP_ADD,.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,.alphaBlendOp = VK_BLEND_OP_ADD,.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT };
    const VkPipelineColorBlendStateCreateInfo colorBlending = {.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,.logicOpEnable = VK_FALSE,.logicOp = VK_LOGIC_OP_COPY,.attachmentCount = 1,.pAttachments = &colorBlendAttachment,.blendConstants = {0.0f,0.0f,0.0f,0.0f}};
    const VkPipelineDepthStencilStateCreateInfo depthStencil = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,.depthTestEnable = static_cast<VkBool32>(useDepth ? VK_TRUE : VK_FALSE),.depthWriteEnable = static_cast<VkBool32>(useDepth ? VK_TRUE : VK_FALSE),.depthCompareOp = VK_COMPARE_OP_LESS,.depthBoundsTestEnable = VK_FALSE,.minDepthBounds =0.0f,.maxDepthBounds=1.0f};
    const VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_SCISSOR};
    const VkPipelineDynamicStateCreateInfo dynamicState = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,.dynamicStateCount = 1,.pDynamicStates = dynamicStates};
    const VkGraphicsPipelineCreateInfo pipelineInfo = {.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,.stageCount = static_cast<uint32_t>(shaderStages.size()),.pStages=shaderStages.data(),.pVertexInputState = &vertexInputInfo,.pInputAssemblyState=&inputAssembly,.pTessellationState = nullptr,.pViewportState=&viewportState,.pRasterizationState=&rasterizer,.pMultisampleState = &multisampling,.pDepthStencilState = &depthStencil,.pColorBlendState=&colorBlending,.pDynamicState = dynamicScissorState ? &dynamicState : nullptr,.layout=pipelineLayout,.renderPass = renderPass,.subpass=0,.basePipelineHandle=VK_NULL_HANDLE,.basePipelineIndex=-1};
    VK_CHECK(vkCreateGraphicsPipelines(vkDev.device,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,pipeline));
    for(auto& sm : shaderModules)
        vkDestroyShaderModule(vkDev.device,sm.shaderModule,nullptr);
    return true;}
