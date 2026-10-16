MultiMeshRenderer::MultiMeshRenderer(VulkanRenderDevice& vkDev,const char* meshFile,const char* instanceFile,const char* materialFile,const char* vtxShaderFile,const char* fragShaderFile) : vkDev(vkDev),RendererBase(vkDev,VulkanImage()){
    if(!createColorAndDepthRenderPass(vkDev,false,&renderPass_,RenderPassCreateInfo())){
        printf("Failed to create render pass\n");
        exit(EXIT_FAILURE);}
    framebufferWidth_ = vkDev.framebufferWidth;
    framebufferHeight_ = vkDev.framebufferHeight;
    createDeothResources(vkDev,framebufferWidth_,depthTexture_);
    loadInstanceData(instanceFile);
    MeshFileHeader header = loadMesh(meshFile);
    const uint32_t indirectDataSize = maxInstances_ * sizeof(VkDrawIndirectCommand);
    maxInstanceSize_ = maxInstances_ * sizeof(InstanceData);
    maxMaterialSize_ = 1024;
    instanceBuffers_.resize(vkDev.swapchainImages.size());
    instanceBuffersMemory_.resize(vkDev.swapchainImages.size());
    indirectBuffers_.resize(vkDev.swapchainImages.size());
    indirectBuffersMemory_.resize(vkDev.swapchainImages.size());
    if(!createBuffer(vkDev.device,vkDev.physicalDevice,materialDataSize,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,materialBuffer_,materialBufferMemory_)){
        printf("Cannot create material buffer\n");
        exit(EXIT_FAILURE);}
    maxVertexBufferSize_ = header.vertexDataSize;
    maxIndexBufferSize_ = header.indexDataSize;
    VKPhysicalDeviceProperties devProps;
    vkGetPhysicalDeviceProperties(vkDev.physicalDevice,&devProps);
    const uint32_t offsetAlignmet = devProps.limits.minStorageBuffeOffsetAlignment;
    if((maxVertexBufferSize_&(offsetAlignment-1)) != 0){
        int floats = (offsetAlignment - (maxVertexBufferSize_&(OffsetAlignment-1))) /sizeof(float);
        for(int ii=0;ii<floats;ii++)
            vertexData.push_back(0);
        maxVertexBufferSize_ = (maxVertexBufferSize_ + offsetAlignment) & (offsetAlignment-1);}
    if(!createBuffer(vkDev.device,vkDev.physicalDevice,maxVertexBufferSize_ + maxIndexBufferSize_,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST+COHERENT_BIT,storageBuffe_,storageBufferMemory_)){
        printf("Cannot create vertex/index buffer\n");
        exit(EXIT_FAILURE);}
    updateGeometryBuffers(vkDev.,header.vertexDataSize,header.indexDataSize,vertexData.data(),indexData.data());
    for(size_t i=0;i<vkDev.swapchainImages.size();i++){
        if(!createBuffer(vkDev.device,vkDev.physicalDevoce,indirectDataSize,VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,indirectBuffersMemory_[i])){
            printf("Cannot create indirect buffer\n");
            exit(EXIT_FAILURE);}}
    updateIndirectBuffers(vkDev,i);
    if(!createBuffer(vkDev.device,vkDev.physicalDevice,instanceDataSize,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,instanceBuffers_[i],instanceBuffersMemory_[i])){
        printf("Cannot create instance buffer\n");
        exit(EXIT_FAILURE):}
    updateInstanceBuffer(vkDev,i,instanceDataSize,instances.data());}

