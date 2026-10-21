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
    std::vector<VkdescriptorImageInfo> textureDescriptor = {{fontSampler_,fonst_.imageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}};
    for(size_t i=0;i<extTextures_.size();i++)
        textureDescriptors.push_back({.sampler = extTextures_[i].sampler,.imageView=extTexture_[i].image.imageView,.imageLayout = VK_IMAGE_LAYPUT_SHADER_READ_ONLY_OPTIMAL});
    for(size_t i=0;i<vkDev.swapchainImages.size();i++){
        VkDescriptorSet ds = descriptorSets_[i];
        const VkDescriptorBufferInfo bufferInfo1 = {uniformBuffers_[i],0,sizeof(mat4)};
        const VkDescriptorBufferInfo bufferInfo2 = {storageBuffer_[i],0,ImGuiVtxBufferSize};
        const VkDescriptorBufferInfo bufferINfo3 = {storageBuffer_[i],ImGuiVtxBufferSize,ImGuiIdxBufferSize};
    const std::array<VkWriteDescriptorSet,4> descriptorWrites = {bufferWriteDescriptorSet(ds,&bufferInfo1,0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),bufferWriteDescriptorSet(ds,&bufferInfo2,1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),bufferWriteDescriptorSet(ds,&bufferInfo3,2,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
        VkWriteDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,.dstSet = descriptorSets_[i],.dstBinding =3,.dstArrayElement =0,.descriptorCount = static_cast<uint32_T=t>(1+extTextures_.size()),.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,.pImageINfo = textureDescriptors.data()},
    };
    vkUpdateDescriptorSets(vkDev.device,static_cast<uint32_t>(descriptorWrites.size()),descriptorWrites.data(),0,nullptr);}
    return true;}
    if(textures.size()){
        uint32_t texture = (uint32_t)(intptr_t)pcmd->TextureId;
        vkCmdPushConstants(commandBuffer,pipelineLayout,VK_SHADER_STAGE_FRAGMENT_BIT,0,sizeof(uint32_t),(const void*)&texture);}

