class RendererBase{
    public:
        explicit RendererBase(const VulkanRenderDevice& vkDev,VulkanImage depthTexture) : device_(vkDev.device),framebufferWidth_(vkDev.framebufferWidth),framebufferHeight_(vkDev.framebufferHeight),depthTexture_(depthTexture) {} virtual ~RendererBase();
        virtual void fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage) =0;
    inline VulkanImage getDepthTexture() const
    {return depthTexture_;}
    protected:
        void beginRenderPass(VkCommandBuffer commandBuffer,size_t currentImage);
