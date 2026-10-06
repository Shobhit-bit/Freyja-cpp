class ModelRenderer:public RendererBase{
public:
    ModelRenderer(VulkanRenderDevice& vkDev,const char* modelFile,const char* textureFile,uint32_t unoformDataSize);
    virtual ~ModelRenderer();
    virtual void fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage) override;
    void updateUniformBuffer(VulkanRenderDevice& vkDev,uint32_t currentImage,const void* data,size_t dataSize);
private:
    size_t vertexBufferSize_;
    size_t indexBufferSize_;
    VkBuffer storageBuffer_;
    VkDeviceMemory storageBufferMemory_;
    VkSampler textureSampler_;
    VulkanImage texture_;
    bool createDescriptorSet(VulkanRenderDevice& vkDev,uint32_t uniformDataSize);};
ModelRenderer::ModelREnderer(VulkanRenderDevice& vkDev,const char* modelFile,const char* textureFile,uint32_t uniformDataSize) : RendererBase(vkDev,VulkanImage()){
    if(!createTexturedVertexBuffer(vkDev,modelFile,&storeageBuffer_,&storageBufferMemory_,&vertexBufferSize_,&indexBufferSize_)){
    printf("ModelRenderer: createTexturedVertexBuffer failed\n");
    exit(EXIT_FAILURE);}
    createTextureImage(vkDev,textureFile,texture_,image,texture_.imageMemory);
    createImageView(vkDev.device,texture_.image,VK_FORMAT_R8G8B8A8_UNORM,VK_IMAGE_ASPECT_COLOR_BIT,&texture_.imageView);
    createTextureSampler(vkDev.device,&textureSampler_);}
bool ModelRenderer :: createDescriptor
