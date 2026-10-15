class ModelRenderer:public RendererBase{
public:
    ModelRenderer(VulkanRenderDevice& vkDev,const char* modelFile,const char* textureFile,uint32_t uniformDataSize);
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
ModelRenderer::ModelRenderer(VulkanRenderDevice& vkDev,const char* modelFile,const char* textureFile,uint32_t uniformDataSize) : RendererBase(vkDev,VulkanImage()){
    // the demo reuses this renderer's depth buffer for every other renderer, so it is created here
    createDepthResources(vkDev,vkDev.framebufferWidth,vkDev.framebufferHeight,depthTexture_);
    if(!createTexturedVertexBuffer(vkDev,modelFile,&storageBuffer_,&storageBufferMemory_,&vertexBufferSize_,&indexBufferSize_)){
        printf("ModelRenderer: createTexturedVertexBuffer failed\n");
        exit(EXIT_FAILURE);}
    createTextureImage(vkDev,textureFile,texture_.image,texture_.imageMemory);
    createImageView(vkDev.device,texture_.image,VK_FORMAT_R8G8B8A8_UNORM,VK_IMAGE_ASPECT_COLOR_BIT,&texture_.imageView);
    createTextureSampler(vkDev.device,&textureSampler_);
    if(!createColorAndDepthRenderPass(vkDev,true,&renderPass_,RenderPassCreateInfo()) ||
       !createUniformBuffers(vkDev,uniformDataSize) ||
       !createColorAndDepthFramebuffers(vkDev,renderPass_,depthTexture_.imageView,swapchainFramebuffers_) ||
       !createDescriptorPool(vkDev,1,2,1,&descriptorPool_) ||
       !createDescriptorSet(vkDev,uniformDataSize) ||
       !createPipelineLayout(vkDev.device,descriptorSetLayout_,&pipelineLayout_) ||
       !createGraphicsPipeline(vkDev,renderPass_,pipelineLayout_,{"data/shaders/chapter04/VK02.vert","data/shaders/chapter04/VK02.frag","data/shaders/chapter04/VK02.geom"},&graphicsPipeline_)){
        printf("ModelRenderer: failed to create pipeline\n");
        exit(EXIT_FAILURE);}}
ModelRenderer::~ModelRenderer(){
    vkDestroyBuffer(device_,storageBuffer_,nullptr);
    vkFreeMemory(device_,storageBufferMemory_,nullptr);
    vkDestroySampler(device_,textureSampler_,nullptr);
    destroyVulkanImage(device_,texture_);}
bool ModelRenderer :: createDescriptorSet(VulkanRenderDevice& vkDev,uint32_t uniformDataSize){
    const std::array<VkDescriptorSetLayoutBinding,4> bindings = {descriptorSetLayoutBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),descriptorSetLayoutBinding(2,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),descriptorSetLayoutBinding(3,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT)};
    const VkDescriptorSetLayoutCreateInfo layoutInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,.pNext = nullptr,.flags=0,.bindingCount = static_cast<uint32_t> (bindings.size()),.pBindings=bindings.data()};
    VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device,&layoutInfo,nullptr,&descriptorSetLayout_));
    std::vector<VkDescriptorSetLayout>
        layouts(vkDev.swapchainImages.size(),descriptorSetLayout_);
    const VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,.pNext = nullptr,.descriptorPool=descriptorPool_,.descriptorSetCount = static_cast<uint32_t>(vkDev.swapchainImages.size()),.pSetLayouts = layouts.data()};
    descriptorSets_.resize(vkDev.swapchainImages.size());
    VK_CHECK(vkAllocateDescriptorSets(vkDev.device,&allocInfo,descriptorSets_.data()));
    for(size_t i=0;i<vkDev.swapchainImages.size();i++){
        VkDescriptorSet ds = descriptorSets_[i];
        const VkDescriptorBufferInfo bufferInfo = {uniformBuffers_[i],0,uniformDataSize};
        const VkDescriptorBufferInfo bufferInfo2 = {storageBuffer_,0,vertexBufferSize_};
        const VkDescriptorBufferInfo bufferInfo3 = {storageBuffer_,vertexBufferSize_,indexBufferSize_};
        const VkDescriptorImageInfo imageInfo = {textureSampler_,texture_.imageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        const std::array<VkWriteDescriptorSet,4> descriptorWrites = {bufferWriteDescriptorSet(ds,&bufferInfo,0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
            bufferWriteDescriptorSet(ds,&bufferInfo2,1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
            bufferWriteDescriptorSet(ds,&bufferInfo3,2,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
            imageWriteDescriptorSet(ds,&imageInfo,3)};
        vkUpdateDescriptorSets(vkDev.device,static_cast<uint32_t>(descriptorWrites.size()),descriptorWrites.data(),0,nullptr);}
    return true;}
void ModelRenderer::updateUniformBuffer(VulkanRenderDevice& vkDev,uint32_t currentImage,const void* data,const size_t dataSize){
    uploadBufferData(vkDev,uniformBuffersMemory_[currentImage],0,data,dataSize);}
void ModelRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage){
    beginRenderPass(commandBuffer,currentImage);
    vkCmdDraw(commandBuffer,static_cast<uint32_t>(indexBufferSize_/sizeof(uint32_t)),1,0,0);
    vkCmdEndRenderPass(commandBuffer);}
class CubeRenderer: public RendererBase{
public :
CubeRenderer(VulkanRenderDevice& vkDev,VulkanImage inDepthTexture,const char* textureFile);
virtual ~CubeRenderer();
virtual void fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage)override;
void updateUniformBuffer(VulkanRenderDevice& vkDev,uint32_t currentImage,const mat4& m);
private:
    VkSampler textureSampler;
    VulkanImage texture;
    bool createDescriptorSet(VulkanRenderDevice& vkDev);};
CubeRenderer::CubeRenderer(VulkanRenderDevice& vkDev,VulkanImage inDepthTexture,const char* textureFile) : RendererBase(vkDev,inDepthTexture){
    createCubeTextureImage(vkDev,textureFile,texture.image,texture.imageMemory);
    createImageView(vkDev.device,texture.image,VK_FORMAT_R32G32B32A32_SFLOAT,VK_IMAGE_ASPECT_COLOR_BIT,&texture.imageView,VK_IMAGE_VIEW_TYPE_CUBE,6);
    createTextureSampler(vkDev.device,&textureSampler);
    if(!createColorAndDepthRenderPass(vkDev,true,&renderPass_,RenderPassCreateInfo()) || !createUniformBuffers(vkDev,sizeof(mat4)) || !createColorAndDepthFramebuffers(vkDev,renderPass_,depthTexture_.imageView,swapchainFramebuffers_) || !createDescriptorPool(vkDev,1,0,1,&descriptorPool_) || !createDescriptorSet(vkDev) || !createPipelineLayout(vkDev.device,descriptorSetLayout_,&pipelineLayout_) || !createGraphicsPipeline(vkDev,renderPass_,pipelineLayout_,{"data/shaders/chapter04/VKCube.vert","data/shaders/chapter04/VKCube.frag"},&graphicsPipeline_)){
    printf("CubeRenderer : failed to create pipeline\n");
    exit(EXIT_FAILURE);}}
CubeRenderer::~CubeRenderer(){
    vkDestroySampler(device_,textureSampler,nullptr);
    destroyVulkanImage(device_,texture);}
void CubeRenderer::updateUniformBuffer(VulkanRenderDevice& vkDev,uint32_t currentImg,const mat4& m){
    uploadBufferData(vkDev,uniformBuffersMemory_[currentImg],0,glm::value_ptr(m),sizeof(mat4));}
void CubeRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage){
    beginRenderPass(commandBuffer,currentImage);
    vkCmdDraw(commandBuffer,36,1,0,0);
    vkCmdEndRenderPass(commandBuffer);}
bool CubeRenderer:: createDescriptorSet(VulkanRenderDevice& vkDev){
    const std::array<VkDescriptorSetLayoutBinding,2> bindings = {
        descriptorSetLayoutBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),descriptorSetLayoutBinding(1,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT)};
    const VkDescriptorSetLayoutCreateInfo layoutInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,.pNext = nullptr,.flags=0,.bindingCount = static_cast<uint32_t>(bindings.size()),.pBindings=bindings.data()};
    VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device,&layoutInfo,nullptr,&descriptorSetLayout_));
    std::vector<VkDescriptorSetLayout> layouts(vkDev.swapchainImages.size(),descriptorSetLayout_);
    const VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,.pNext = nullptr,.descriptorPool=descriptorPool_,.descriptorSetCount = static_cast<uint32_t>(vkDev.swapchainImages.size()),.pSetLayouts = layouts.data()};
    descriptorSets_.resize(vkDev.swapchainImages.size());
    VK_CHECK(vkAllocateDescriptorSets(vkDev.device,&allocInfo,descriptorSets_.data()));
    for(size_t i=0;i<vkDev.swapchainImages.size();i++){
        VkDescriptorSet ds = descriptorSets_[i];
        const VkDescriptorBufferInfo bufferInfo = {uniformBuffers_[i],0,sizeof(mat4)};
        const VkDescriptorImageInfo imageInfo = {textureSampler,texture.imageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        const std::array<VkWriteDescriptorSet,2> descriptorWrites = {bufferWriteDescriptorSet(ds,&bufferInfo,0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
            imageWriteDescriptorSet(ds,&imageInfo,1)};
        vkUpdateDescriptorSets(vkDev.device,static_cast<uint32_t>(descriptorWrites.size()),descriptorWrites.data(),0,nullptr);}
    return true;}
