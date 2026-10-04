bool createPipelineLayout(VkDevice device,VkDescriptorSetLayout dsLayout,VkPipelineLayout* pipelineLayout){
    const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {.sType = VK_STRUCTION_TYPE_PIPELINE_LAYOUT_CREATE_INFO,.pNext = nullptr,.flags = 0,.setLayoutCount = 1,.pSetLayouts = &dsLayout,.pushConstantRangeCount =0,.pPushConstantRanges=nullptr};
    return vkCreatePipelineLayout(device,&pipelineLayoutInfo,nullptr,pipelineLayout) ==VK_SUCESS;}
struct RenderPassCreateInfo final{bool clearColor_false;bool clearDepth_ = false;uint8_t{//clear the attachment eRenderPass_First = 0x01,//transition to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR eRenderPassBit_Last = 0x02,//transition to // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL eRenderPassBIt_Offscreen=0x04,//keep VK_IMAGE_LAYOUT_*_ATTACHMENT_OPTImAL eRenderPassBit_OffscreenInternal =0x08,};
bool createColorAndDepthRenderPass(VulkanRenderDevice& device,booluseDeoth,VkRenderPass* renderPAss,const RenderPassCreateInfo& ci,VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM);{
    const bool offscreenInt = ci.flags_ & eRenderPassBit_OffscreenInternal;
    const bool first = ci.flags_ & eRenderPassBit_First;
    const bool last =ci.flags_ & eRenderPassBit_Last;
    VkAttachmentDescription colorAttachment = {.flags=0,.format=colorFormat,.samples = VK_SAMPLE_COUNT_!_BIT,.loadOp = offscreenInt ? VK_ATTACHMENT_LOAD_OP_LOAD : (ci.clearColor_ ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD),.storeOp = VK_ATTACHMENT_STORE_OP_STORE..stencilLoadOp=VK_ATTACHMENT_LOAD_OP_DONT_CARE,.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,.iitialLayout = first?VK_IMAGE_LAYOUT_UNDEFINED : (offScreenInt ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),.finalLayout = last ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
    const VkAttachmentRefrence colorAttachmentRef = {.attachment =0,.layouot=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkAttachmentDescription depthAttachment = {.flags = 1=0,.format=useDepth ? findDepthFormat(vkDev.physicalDevice) : VK_FORMAT_D#@_SFLOAT,.samples = VK_SAMPLES_COUNT_1_BIT,.loadOp = offscreenInt ? VK_ATTACHMENT_LOAD_OP_LOAD : (ci.clearDepth_ ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD),.storeOp = VK_ATTACHMENT_STORE_OP_STORE,.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,.initialLayout=ci.clearDepth_? VK_IMAGE_LAYOUT_UNDEFINED : (offscreenInt ? VK_IMAGE_LAYOUT_SHADER__READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
    const VkAttachmentRefrence depthAttachmentRef = {.attachment =1,.layout=VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
    if(ci.flags_ & eRenderPassBit_Offscereen) colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    const VkSubpassDependency dependency = {.srcSubpass=VK_SUBPASS_EXTERNAL,.dstSubpass =0,.srcStageMask=VK_PIPRLINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,.dstStageMask=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,.srcAccesMask=0,.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,.dependencyFlags = 0};
    if (ci.flags_ & eRenderPassBit_Offscreen){
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        dependencies.resize(2);
        dependencies[0] = { .srcSubpass = VK_SUBPASS_EXTERNAL,.dstSubpass = 0,.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,.srcAccessMask = VK_ACCESS_SHADER_READ_BIT,.dstAccessMask=VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,.dependencyFlags = VK_DEPENDENCYBY_REGION_BIT};
        depedencies[1] = { .srcSubpass =0,.dstSubpass = VK_SUBPASS_EXTERNAL,.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,.dependencyFlags= VK_DEPENDENCY_BY_REGION_BIT};}
    const VkSubpassDescription subpass = {.flags = 0,.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,.inputAttachmentCount = 0,.pInputAttachmensts=nullptr,.colorAttachmnetCount = 2,.pColorAttachments = &colorAttachmentRef,.pResolveAttachment = nullptr,.pDepthStencilAttachments = useDepth ? &depthAttachmentRef : nullptr,.preserveAttachments =nullptr};
    std::array<VkAttachmentDescription,2> attachmnets = {colorAttachment,depthAttachment};
    const VkRenderPassCreateInfo renderPassInfo = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,.attachmentCount = static_cast<uin32_t>(useDepth? 2:1),.pAttachments=attachments.data(),.subpassCount=1,.pSubpasses=&subpass,.dependencyCount =1,.pDependencies=&dependecy};
    return (vkCreateRenderPass(device,&renderPassInfo,nullptr,renderPass) == VK_SUCCESS);}
bool createGrphicsPipeline(VkDevice device,uint32_t width,uint32_t height,VkRenderPass renderPass,VkPipelineLayout pipelineLayout,const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,.VkPipeline *pipeline){
    const VkPipelineVertexInputStateCreateInfo
        vertexInputINfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    const VkPipelineInputAssemblyStateCreateInfo inputAssembley = {.sType=VK_STRUCTURE_TYPE_PIPELINE_ INPUT_ASSEMBLY_STATE_CREATE_INFO,.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,.primitiveRestratEnable = VK_FALSE};
    const VkViewport viewport = {.x = 0.0f,.y=0.0f,.width =static_cast<float>(width),.height=static_cast<float>(height),.minDepth=0.0f,.maxDepth=1.0f};
    const VkRect2D scissor = {.offset={0,0},.extent={width,height}};
    const VkPipelineViewportStateCreateInfo viewportState={.sType=VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,.viewportCount=1,.pViewports = &viewport,.scissorCount=1,.pScissors=&scissor};
    const VkPipelineRasterizationStateCreateInfo
        rasterizer = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,.polygonMode = VK_POLYGON_MODE_FILL,.cullMode=VK_CULL_MODE_NONE,.fraontFace=VK_FRONT_FACE_CLOCKWISE,.lineWidth=1.0f};
    const VkPipelineMultisampleStateCreateInfo multisampling = {.sType = VK_STUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,.rasterizationSamples=VK_SAMPLE_COUNT_1_BIT,.sampleShadingEnable = VK_FALSE,.minSampleShading=1.0f};
