int main(){
    glslang_initialize_process();
    volkInitialize();
    if(!glfwInit())
        exit(EXIT_FAILURE);
    if(!glfwVulkanSupported())
        exit(EXIT_FAILURE);
    const uint32_t kScreenWidth =1280;
    const uint32_t kScreenHeight = 720;
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    window = glfwCreateWindow(kScreenWidth,ScreenHeight,"VulkanApp",nullptr,nullptr);
initVulkan();
while(!glfwWindowShouldClose(window)){
    drawOverlay();
    glfwPollEvents();}
terminateVulkan();
glfwTerminate();
glslang_finalize_process();
return 0;}
bool initVulkan(){
    createInstance(&vk.instance);
    if(!setupDebugCallbacks(vk.instance,&vk.messenger,&vk.reportCallback))
        exit(EXIT_FAILURE);
    if(glfwCreateWindowSurface(vk.instance,window,nullptr,&vk.surface))
        exit(EXIT_FAILURE);
    if(!inntVulkanRenderDevice(vk,vkDev,kScreenWidth,kScreenHeight,isDeviceSuitable,{.geometryShader=VK_TRUE}))
        exit(EXIT_FAILURE);
    VK_CHECK(createShaderMOdule(vkDev.device,&vkState.vertShader,"data/shaders/VK02.vert"));
    VK_CHECK(createShaderModule(vkDev.device,&vkState.fragShader,"data/shader/VK02.frag"));
    VK_CHECK(createShaderMPdule(vkDev.device,&vkState.geomShader,"data/shader/VK02.geom"));
    if(!createTexturedVertexBuffer(vkDev,"data/rubber_duck/scene.gltf",&vkState.storageBuffer,&vkState.storageBufferMemory,&vertexBufferSize,&indexBufferSize) || !createUniformBuffers()){
        printf("Cannot create data buffers\n");
        exit(EXIT_FAILURE);}
    const std::vector<VkPipelineShaderStageCreateInfo>
        shaderStages = { shaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT,vkState.vertShader,"main"),
            shaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT,vkState.fragShader,"main"),
            shaderStafeInfo(VK_SHADER_STAGE_GEOMETRY_BIT,vkState.geomShader,"main")};
    createTextureImage(vkDev,"data/rubber_duck/textures/Duck_baseColor.png",vkState.texture.image,vkState.texture.imageMemory);
    createImageView(vkDev.device,vkState.texture.image,VK_FORMAT_R8B8G8A8_UNORM,VK_IMAGE_ASPECT_COLOR_BIT,&vkState.texture.imageView);
    createTextureSampler(vkDev.device,&vkState.textureSampler);
    createDepthResources(vkDev,kScreenWidth,kScreenHeight,vkState.depthTexture);
    const bool isIntialized = createDescriptorPool(vkDev.device,static_cast<uint32_t>(vkDev.swapchainImages.size()),1,2,1,&vkState.descriptorPool) && createDescriptorSet() && createColorAndDepthRenderPass(vkDev,true,&vkState.renderPass,RenderPassCreateInfo{.clearColor_ = true,.clearDepth_=true,.flags_ =eRenderPassBit_First|eRenderPassBit_Last}) && createPipelineLayout(vkDev.device,vkStatedescriptorSetLayout,&vkState.pipelineLayout) && createGraphicsPipeline(vkDev.device,kScreenWidth,kScrrenHeight,vkState.renderPass,vkState.pipelineLayout,shaderStages,&vkState.graphicsPipeline);
    if(!isInitialized){
        printf("failed to pipeline\n");
        exit(EXIT_FAILURE);}
    createColorAndDepthFramebuffers(vkDev,vkState.renderPass,vkState.depthTexture.imageView,kScreenWidth,kScreenHeight,vkState.swapchainFramebuffers);
    return VK_SUCCESS;}
bool drawOverlay(){
    uint32_t imageIndex =0;
    VK_CHECK(vkAcquireNextImageKHR(vkDev.device,vkDev.swapchain,0,vkDev.semaphore,VK_NULL_HANDLE,&imageIndex);
            VK_CHECK(vkResetCommandPool(vkDev.device,vkDev.commandPool,0));
    int width,height;
    glfwGetFramebufferSize(window,&width,&height);
    const float ratio = width/(float)height;
    const mat4 ml = glm::rotate(glm::translate(mat4(1.0f),vec3(0.f,0.5f,-1.5f)) * glm::rotate(mat4(1.f),glm::pi<float>(),vec3(1,0,0)),(float)glfwGetTime(),vec3(0.0f,1.0f,0.0f));
    const mat4 p =glm::persetective(45.0f,ratio,0.1f,1000.0f);
    const UniformBuffer ubo{.mvp = p*ml};
    updateUniformBuffer(imageIndex,ubo);
    fillCommandBuffer();
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    const VkSubmitInfo si ={.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO,.pNext = nullptr,.waitSemaphoreCount = 1,.pWaitSemaphore=&vkDev.semaphore,.pWaitDstStageMask=waitStages,.commandBufferCount=1,.pCommandBuffers=&vkDev.commandBuffer[imageIndex],.signalSemaphoreCount=1,.pSignalSemaphore = &vkDev.renderSemaphore};
    VK_CHECK(vkQueueSubmit(vkDev.graphicsQueue,1,&si,nullptr));
    const VkPresentInfoKHR pi= {.sType=VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,.pNext=nullptr,.pWaitSemaphore
