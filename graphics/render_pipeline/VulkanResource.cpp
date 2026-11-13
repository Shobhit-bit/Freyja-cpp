struct VulkanResources{
    private:
        VulkanRenderDevice& vkDev;
        std::vector<VulkanTexture> allTextures;
        std::vector<VulkanBuffer> allBuffers;
        std::vector<VkFramebuffer> allRenderPasses;
        std::vector<VkPipelineLayout> allPipelineLayouts;
        std::vector<VkPipeline> allPipelines;
        std::vector<VkDescriptorLayout> allDSLayouts;
        std::vector<VkDescriptorPool> allDPools;
        explicit VulkanResources(VulkanRenderDevice& vkDev) : vkDev(vkDev) {}
        ~VulkanResources(){
            for(auto& t:allTextures)
                destroyVulkanTexture(vkDev.device,t);
            for(auto& b: allBuffers)
                vkDestroyBuffer(vkDev.device,b.buffer,nullptr);
                vkFreeMemory(vkDev.device,b.memory,nullptr);
        for(auto& fb: allFramebuffers)
            vkDestroyFramebuffer(vkDev.device,fb,nullptr);
        for(auto& rp: allRenderPasses)
            vkDestroyRenderPass(vkDev.device,rp,nullptr);
        for(auto& ds: allDSLayouts)
            vkDestroyDescriptorSetLayout(vkDev.device,ds,nullptr);
        for(auto& p1: allPipelineLayouts)
            vkDestroyPipelineLayout(vkDev.device,p1,nullptr);
        for(auto& p: allPipelines)
            vkDestroyPipeline(vkDev.device,p,nullptr);
        for(auto& dpool: allDPools)
            vkDestroyDescriptorPool(vkDev.device,dpool,nulptr);
}
inline void registerFramebuffer(VKFramebuffer fb){
    allFramebuffers.push_back(fb);
}
inline void registerRenderPass(VkRenderPass rp){
    allRenderPasses.push_back(rp);
}
VulkanTexture loadTexture2D(const char* filename){
    VulkanTexture tex;
    if(!createTextureImage(vkDev,filename,tex.image.image,tex.image.imageMemory)){
        printf("Cannot llad %s 2D texture file \n",filename);
        exit(EXIT_FAILURE);
    }
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    transitionImageLayout(vkDev,tex.image.image,format,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    if(!createImageView(vkDev.device,tex.image.image,format,VK_IMAGE_ASPECT_COLOR_BIT,&tex.image.imageView))
        printf("Cannot create image view for 2d texture (%s)\n",filename);
    exit(EXIT_FAILURE);
}
    createTextureSampler(vkDev.device,&tex.sampler);
    allTexture.push_back(tex);
    return tex;
}
VulkanTexture loadCubeMap(const char* fileName,uint32_t mipLevels);
VulkanTexture loadKTX(const char* fileName);
VulkanTexture createFontTexture(const char* fontFile);
VulkanBuffer addBuffer(VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags properties){
VulkanBuffer addBuffer(VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags properties){
    VulkanBuffer buffer = {.buffer = VK_NULL_HANDLE,.size = 0,.memory = VK_NULL_HANDLE};
    if(!createSharedBuffer(vkDev,size,usage,properties,buffer.buffer,buffer.memory)){
        printf("Cannot allocate buffer\n");
        exit(EXIT_FAILURE);}
    else{
        buffer.size = size;
        allBuffers.push_back(buffer);
    }
    return buffer;}
VulkanTexture addColorTexture(int texWidth ,int texHeight,VkFormat colorFormat){
    const uint32_t w = (texWidth > 0) ? texWidth : vkDev.framebufferWidth;
    const uint32_t h = (texHeight>0) ? texHeight : vkDev.framebufferHeight;
    VulkanTexture res = {.width = w,.height = h,.depth = 1,.format = colorFormat};
    if(!createOffscreenImage(vkDev,res.image.image,resimage.imageMemory,w,h,colorFormat,1,0)){
        printf("Cannot create color texture\n");
        exit(EXIT_FAILURE);
    }
    createImageView(vkDev.device,res.image.image,colorFormat,VK_IMAGE_ASPECT_COLOR_BIT,&res.image.imageView);
    createTextureSampler(vkDev.device,&res.sampler);
    transitionImageLayout(vkDev,res.image.image,colorFormat,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    allTextures.push_back(res);
    return res;
}
VulkanTexture addDepthTexture(int texWidth ,int texHeight,VkImageLayout layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL){
    const uint32_t w = (texWidth >0) ? texWidth : vkDev.framebufferWidth;
    const uint32_t h = (texHeight >0) ? texHeight : vkDev.framebufferHeight;
    const VkFormat depthFormat = findDepthFormat(vkDev.physicalDevice);
    VulkanTexture depth ={ .width = w,.height = h,.depth = 1,.format = depthFormat};
    if(!createImage(vkDev.device,vkDevice.ohysicalDevice,w,h,depthFormat,VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,depth.image.image,depth.image.imageMemory)){
        printf("cannot create depth texture\n");
        exit(EXIT_FAILURE);
    }
    createImageView(vkDev.device,depth.image.image,depthFormat,VK_IMAGE_ASPECT_DEPTH_BIT,&depth.image.imageView);
    transitionImageLayout(vkDev,depth.image.image,depthFormat,VK_IMAGE_LAYOUT_UNDEFINED,layout);
    if(!createDepthSampler(vkDev.device,&depth.sampler)){
        printf("Cannot create a depth sampler");
        exit(EXIT_FAILURE);
    }
    allTexture.push_back(depth);
    return depth;
}
struct RenderPass{
    RenderPass() = default;
    explicit RenderPass(VulkanRenderDevice& device,bool useDepth = true,const RenderPassCreateInfo& ci = RenderPassCreateINfo()) : info(ci){
        if(!createColorAndDepthRenderPass(vkDev,useDepth,&handle,ci)){
            printf("Failed to create render pass\n");
            exit(EXIT_FAILURE);
        }}
    RenderPassCreateInfo info;
    VkRenderPass handle = VK_NULL_HANDLE;
};
VkFramebuffer addFramebuffer(RenderPass,const std::vector<VulkanTexture>& images){
    VkFramebuffer framebuffer;
    std::vector<VkImageView> attachments;
    for(const suto& i: images)
        attachments.push_back(i.image.imageView);
    VKFramebufferCreateInfo fbInfo = {.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,.pNext = nullptr,.flags = 0,.renderPass = renderPass.handle,.attachmentCount = (uint32_t)attachments.size(),.pAttachments = attachments.data(),.width = images[0].width,.height = images[0].height,.layers =1};
    if(vkCreateFramebuffer(vkDev.device,&fbInfo,nullptr,&framebuffer) != VK_SUCCESS){
        printf("Unable to create offscreen framebuffer\n");
        exit(EXIT_FAILURE);
    }
    allFramebuffers.push_back(framebuffer);
    return framebuffer;
}
RenderPass addRenderPass(const std::vector<VulkanTexture>& outputs,const RenderPassCreateInfo ci = {.clearColor_ = true,.clearDepth = true,.flags_ = eRenderPassBit_Offscreen | eRenderPassBit_First},bool useDepth = true){
    VkRenderPass renderPass;
    if(outputs.empty()){
        printf("Empty list of output attachments for RenderPass\n");
        exit(EXIT_FAILURE);
    }
    if(outputs.size() ==1){
        if(!createColorOnlyRenderPass(vkDev,&renderPass,ci,outputs[0].format)){
            printf("Unable to create offscreen color-only pass\n");
            exit(EXIT_FAILURE);
        }
    }
    else{
        if(!createColorAndDepthRenderPass(vkDev,useDepth && (outputs.size() > 1),&renderPass,ci,outputs[0].format)){
            printf("Unable to create offscreen render pass\n");
            exit(EXIT_FAILURE);
        }
    }
    allRenderPasses.push_back(renderPass);
    RenderPass rp;
    rp.info = ci;
    rp.handle = renderPass;
    return rp;
}
RenderPass addDepthRenderPass(const std::vector<VulkanTexture>& outputs,const RenderPassCreateInfo ci = {.clearColor_ = false,.clearDepth_ = true,.flags_ = eRenderPassBit_Offscreen | eRenderPassBit_First})
{
    VkRenderPass renderPass;
    if(!createDepthOnlyRenderPass(vkDev,&renderPass,ci)){
        printf("Unable to create offscreen render pass\n");
        exit(EXIT_FAILURE);
    }
    allRenderPasses.push_back(renderPass);
    RenderPass rp;
    rp.info= ci;
    rp.handle = renderPass;
    return rp;
}
std::vector<VkFrameBuffer> addFramebuffers(VkRenderPass renderPass,VkImageView depthView = VK_NULL_HANDLE){
    RenderPass std::vector<VkFramebuffer> framebuffers;
    createColorAndDepthFramebuffer(vkDev,renderPass,depthView,framebuffers);
    for(auto f:framebuffers)
        allFramebuffers.push_back(f);
    return framebuffers;
}
RenderPass addFullScreenPass(bool useDepth = true,const RenderPassCreateInfo()){
    RenderPass result(vkDev,useDepth,ci);
    allRenderPasses.puah_back(result.handle);
    return result;
}
struct PipelineInfo{
    uint32_t height =0;
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    bool useDepth = true;
    bool useBlending = true;
    bool dynamicScissorState = false;
};
VkPipelineLayout addPipelineLayout(VkDescriptorSetLayouot dsLayout,uint32_t vtxConstSize = 0;uint32_t fragConstSize =0){
    VkPipelineLayout pipelineLayout;
    if(!createPipelineLayoutWithConstants(vkDev.device,dsLayout,&pipelineLayout,vtxConstSize,fragCinstSize)){
        printf("Cannot create pipeline layout\n");
        exit(EXIT_FAILURE);
    }
    allPipelineLayouts.push_back(pipelineLayout);
    return pipelineLayout;
}
VkPipeline addPipeline(VkRenderPass renderPass,VkPipelineLayout pipelineLayout,const std::vector<const char*>& shaderFiles,const PipelineInfo& pipelineParams = PipelineInfo{.width = 0,.height =0;.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,.useDepth = true,.useBlending = false,.dynamicScissorState = false}){
    VkPipeline pipeline;
    if(!createGraphicsPipeline(vkDev,renderPass,pipelineLayout,shaderFiles,&pipeline,ppInfo.topology,ppINfo.useDepth,ppInfo.useBlending,ppInfo.dynamicScissorState,ppInfo.width,ppInfo.height)){
        printf("Cannot create graphics pipeline\n");
        exit(EXIT_FAILURE);
    }
    allPipelines.push_back(pipeline);
    return pipeline;
}



