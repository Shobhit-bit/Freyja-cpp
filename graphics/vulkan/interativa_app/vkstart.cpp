class RendererBase{
    public:
        explicit RendererBase(const VulkanRenderDevice& vkDev,VulkanImage depthTexture) : device_(vkDev.device),framebufferWidth_(vkDev.framebufferWidth),framebufferHeight_(vkDev.framebufferHeight),depthTexture_(depthTexture) {}
        virtual ~RendererBase();
        virtual void fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage) =0;
    inline VulkanImage getDepthTexture() const
    {return depthTexture_;}
    protected:
        void beginRenderPass(VkCommandBuffer commandBuffer,size_t currentImage);
        bool createUniformBuffers(VulkanRenderDevice& vkDev,size_t uniformDataSize);
        VkDevice device_;
        uint32_t framebufferWidth_;
        uint32_t framebufferHeight_;
        VkDescriptorSetLayout descriptorSetLayout_;
        VkDescriptorPool descriptorPool_;
        std::vector<VkDescriptorSet> descriptorSets_;
        std::vector<VkFramebuffer> swapchainFramebuffers_;
        VulkanImage depthTexture_;
        VkRenderPass renderPass_;
        VkPipelineLayout pipelineLayout_;
        VkPipeline graphicsPipeline_;
        std::vector<VkBuffer> uniformBuffers_;
        std::vector<VkDeviceMemory> uniformBuffersMemory_;};
bool RendererBase::createUniformBuffers(VulkanRenderDevice& vkDev,size_t uniformDataSize){
    uniformBuffers_.resize(vkDev.swapchainImages.size());
    uniformBuffersMemory_.resize(vkDev.swapchainImages.size());
    for(size_t i=0;i<vkDev.swapchainImages.size();i++){
        if(!createUniformBuffer(vkDev,uniformBuffers_[i],uniformBuffersMemory_[i],uniformDataSize)){
            printf("Cannot create uniform buffer\n");
            return false;}}
    return true;}
void RendererBase::beginRenderPass(VkCommandBuffer commandBuffer,size_t currentImage){
    const VkRect2D screenRect = {.offset={0,0},.extent={.width = framebufferWidth_,.height=framebufferHeight_}};
    const VkRenderPassBeginInfo renderPassInfo = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,.pNext = nullptr,.renderPass = renderPass_,.framebuffer = swapchainFramebuffers_[currentImage],.renderArea = screenRect};
    vkCmdBeginRenderPass(commandBuffer,&renderPassInfo,VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,graphicsPipeline_);
    vkCmdBindDescriptorSets(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,pipelineLayout_,0,1,&descriptorSets_[currentImage],0,nullptr);}
RendererBase::~RendererBase(){
    for(auto buf:uniformBuffers_) vkDestroyBuffer(device_,buf,nullptr);
    for(auto mem : uniformBuffersMemory_) vkFreeMemory(device_,mem,nullptr);
    vkDestroyDescriptorSetLayout(device_,descriptorSetLayout_,nullptr);
    vkDestroyDescriptorPool(device_,descriptorPool_,nullptr);
    for(auto framebuffer : swapchainFramebuffers_)
        vkDestroyFramebuffer(device_,framebuffer,nullptr);
    vkDestroyRenderPass(device_,renderPass_,nullptr);
    vkDestroyPipelineLayout(device_,pipelineLayout_,nullptr);
    vkDestroyPipeline(device_,graphicsPipeline_,nullptr);}
class VulkanClear:public RendererBase{
    public:
        VulkanClear(VulkanRenderDevice& vkDev,VulkanImage depthTexture);
        virtual void fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage) override;
    private:
        bool shouldClearDepth;};
VulkanClear::VulkanClear(
        VulkanRenderDevice& vkDev,VulkanImage depthTexture) : RendererBase(vkDev,depthTexture),shouldClearDepth(depthTexture.image != VK_NULL_HANDLE){
    if(!createColorAndDepthRenderPass(vkDev,shouldClearDepth,&renderPass_,RenderPassCreateInfo{.clearColor_ = true,.clearDepth_ = true,.flags_ =eRenderPassBit_First})){
        printf("VulkanClear: failed to create render pass\n");
        exit(EXIT_FAILURE);}
    createColorAndDepthFramebuffers(vkDev,renderPass_,depthTexture.imageView,swapchainFramebuffers_);}
void VulkanClear::fillCommandBuffer(VkCommandBuffer commandBuffer,size_t swapFramebuffer){
    const std::array<VkClearValue,2> clearValues = {VkClearValue{.color = {{1.0f,1.0f,1.0f,1.0f}}},VkClearValue{.depthStencil = {1.0f,0}}};
    const VkRect2D screenRect = {.offset = {0,0},.extent = {.width = framebufferWidth_,.height=framebufferHeight_}};
    const VkRenderPassBeginInfo renderPassInfo = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,.renderPass = renderPass_,.framebuffer=swapchainFramebuffers_[swapFramebuffer],.renderArea = screenRect,.clearValueCount = shouldClearDepth ? 2u : 1u,.pClearValues = clearValues.data()};
    vkCmdBeginRenderPass(commandBuffer,&renderPassInfo,VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(commandBuffer);}
class VulkanFinish:public RendererBase {
    public:
        VulkanFinish(VulkanRenderDevice& vkDev,VulkanImage depthTexture);
        virtual void fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage) override;};
VulkanFinish::VulkanFinish(
    VulkanRenderDevice& vkDev,VulkanImage depthTexture) : RendererBase(vkDev,depthTexture){
    if(!createColorAndDepthRenderPass(vkDev,(depthTexture.image != VK_NULL_HANDLE),&renderPass_,RenderPassCreateInfo{.clearColor_ = false,.clearDepth_ = false,.flags_ = eRenderPassBit_Last})){
    printf("VulkanFinish:failed to create render pass\n");
    exit(EXIT_FAILURE);}
    createColorAndDepthFramebuffers(vkDev,renderPass_,depthTexture.imageView,swapchainFramebuffers_);}
void VulkanFinish::fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage){
    const VkRect2D screenRect = {.offset = {0,0},.extent = {.width = framebufferWidth_,.height = framebufferHeight_}};
    const VkRenderPassBeginInfo renderPassInfo = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,.renderPass = renderPass_,.framebuffer = swapchainFramebuffers_[currentImage],.renderArea = screenRect};
    vkCmdBeginRenderPass(commandBuffer,&renderPassInfo,VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(commandBuffer);}
