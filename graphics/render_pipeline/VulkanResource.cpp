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

