std::vector<VulkanTexture> extTextures_;
ImGuiRenderer::ImGuiRenderer(VulkanRenderDevice& vkDev,const std::vector<VulkanTexture>& textures) : RendererBase(vkDev,VulkanImage()),extTextures_(textures)
    if(!createColorAndDepthRenderPass(vkDev,false,&renderPass_,RenderPassCreateInfo()) || !createColorAndDepthFramebuffers(vkDev,renderPass_,VK_NULL_HANDLE,swapchainFramebuffers_) || !createUniformBuffers(vkDev,sizeof(mat4)) || !createDescriptorPool(vkDev,1,2,1+textures.size(),&descriptorPool_) || !createMultiDescriptorSet(vkDev) || !createPipelineLayoutWithConstants(vkDev.device,descriptorSetLayout_,&pipelineLayout_,0,sizeof(uint32_t)) || !createGraphicsPipeline(vkDev,renderPass_,pipelineLayout_,{"data/shaders/imgui.vert","data/shaders/ingui_multi.frag"},&graphicsPipeline_,VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,true,true,true)){
    printf("ImGuiRenderer: pipeline creation failed\n");
    exit(EXIT_FAILURE);}
bool ImGuiRenderer::createMultiDescriptorSet(VulkanRenderDevice& vkDev){
    const std::array<VkDescriptorSetLayoutBinding,4> bindings = {descriptorSetLayoutBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(2,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(3,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT,1+extTextures_,size())};
    const VkDescriptorSetLayoutCreateInfo layoutInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,.pNext = nullptr,.flags = 0,.bindingCount = static_cast<uint32_t>(bindings.size()),.pBindings = bindings.data()};
    std::vector<VkDescriptorSetLayout> layouts(vkDev.swapchainImages.size(),descriptorSetLayout_);
    const VkDescriptorSetAllocateInfo allocInfo = {.sType =VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,.pNext = nullptr,.descriptorPool = descriptorPool_,.descriptorSetCount =static_cast<uint32_t>(vkDev.swapchainImages.size()),.pSetLayouts = layouts.data()};
    descriptorSets_.resize(vkDev.swapchainImages.size());
    VK_CHECK(vkAllocateDescriptorSets(vkDev.device,&allocInfo,descriptorSets_.data()));

