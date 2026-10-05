class RendererBase{
    public:
        explicit RendererBase(const VulkanRenderDevice& vkDev,VulkanImage depthTexture) : device_(vkDev.device),framebufferWidth_(vkDev.framebufferWidth),framebufferHeight_(vkDev.framebufferHeight),depthTexture_(depthTexture) {} virtual ~RendererBase();
        virtual void fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage) =0;
    inline VulkanImage getDepthTexture() const
    {return depthTexture_;}
    protected:
        void beginRenderPass(VkCommandBuffer commandBuffer,size_t currentImage);
        bool createUniformBuffers(VulkanRenderDevice& vkDev,size_t uniformDataSize);
        uint32_t framebufferWidth_;
        uint32_t framebufferHeight_;
        VkDescriptorSetLayout descriptorSetLayout_;
        VkDescriptorPool descriptorPool_;
        std::vector<VkDescriptorSet> descriptorSets_;
        std::vector<VkFramebuffer> swapchainFramebuffers_;
        VulkanImage depthTexture_;
        VkRenderPass renderPass_;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline_;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;};
bool RendererBase::createUniformBuffers(VulkanRenderDevice& vkDev,size_t uniformDataSize){
    uniformBuffers_.resize(vkDev.swapchainIMages.size());
    uniformBuffersMemory_.resize(vkDev.swapchainImages.size());
    for(size_t i=0;i<vkDev.swapchaininImages.size();i++){
        if(!createUniformBuffer(vkDev,uniformBuffers_[i],uniformBuffersMemory_[i],uniformDataSize)){
            printf("Cannot create uniform buffer\n");
            return false;}}
    return true;}
oid RendererBase::beginRenderPass(VkCommandBuffer commandBuffer,size_t currentImage){
    const VkRect2D screenRect = {.offset={0,0}.extent={.width = framebufferWidth_,.height=framebufferHeight_}};
    const VkRenderPassBeginInfo renderPassInfo = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,.pNext = nullptr,.renderPass = renderPass_,.framebuffer = swapchainFramebuffers_[currentImage],.renderArea = screenRect};

