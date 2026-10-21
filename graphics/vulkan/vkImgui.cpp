std::vector<VulkanTexture> extTextures_;
ImGuiRenderer::ImGuiRenderer(VulkanRenderDevice& vkDev,const std::vector<VulkanTexture>& textures) : RendererBase(vkDev,VulkanImage()),extTextures_(textures)
    if(!createColorAndDepthRenderPass(vkDev,false,&renderPass_,RenderPassCreateInfo()) || !createColorAndDepthFramebuffers(vkDev,renderPass_,VK_NULL_HANDLE,swapchainFramebuffers_) || !createUniformBuffers(vkDev,sizeof(mat4)) || !createDescriptorPool(vkDev,1,2,1+textures.size(),&descriptorPool_) || !createMultiDescriptorSet(vkDev) || !createPipelineLayoutWithConstants(vkDev.device,descriptorSetLayout_,&pipelineLayout_,0,sizeof(uint32_t)) || !createGraphicsPipeline(vkDev,renderPass_,pipelineLayout_,{"data/shaders/imgui.vert","data/shaders/ingui_multi.frag"},&graphicsPipeline_,VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,true,true,true)){
    printf("ImGuiRenderer: pipeline creation failed\n");
    exit(EXIT_FAILURE);}

