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
bool ModelRenderer :: createDescriptorSet(VulkanRenderDevice& vkDev,uint32_t uniformDataSize){
    const std::array<VkDescriptorSetLayoutBinding,4>    binding = {descriptorSetLayoutBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),descriptorSetLayoutBinding(2,VKDECRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),descriptorSetLayoutBinding(3,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT)};
    const VkDescriptorSetLayoutCreateInfo layoutInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,.pNext = nullptr,.flags=0,.bindingCount = static_cast<uint32_t> (bindings.size()),.pBindings=bindings.data()};
    VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device,&layoutInfo,nullptr,&descriptorSetLayout_));
    std::vector<VkDescriptorSetLayout>
        layouts(vkdev.swapchainImages.size(),descriptorSetLayout_);
    const VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,.pNext = nullptr
