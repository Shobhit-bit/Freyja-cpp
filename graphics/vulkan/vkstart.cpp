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
    vkCmdBeginRenderPass(commandBuffer,&renderPassInfo,VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,graphicsPipeline_);
    vkCmdBindDescriptorSets(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,pipelineLayout_,0,1,&descriptorSets_[currentImage],0,nullptr);}
RenderBase::~RendererBase(){
    for(auto buf:uniformBuffers_) vkDestroyBuffer(device_,buf,nullptr);
    for(auto mem : uniformBuffersMemory_) vkFreeMemory(device_,mem,nullptr);
    vkDestroyDescriptorSetLayout(device_,descriptorSetLayout_,nullptr);
    vkDestroyDescriptorPool(device_,descriptorPool_,nullptr);
    for(auto frameBuffer : swapinFramebuffers_)
        vkDestroyFramebuffer(device_,framebuffer,nullptr);
    vkDestroyRenderPass(device_,renderPass_,nullptr);
    vkDestroyPipelineLayout(device_,pipelineLayout_,nullptr);
    vkDestroyPipeline(device_,graphicsPipeline_,nullptr);}

