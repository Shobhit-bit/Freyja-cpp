void uploadBufferData(const VulkanRenderDevice& vkDev,VkDeviceMemory& bufferMemory,VkDeviceSize deviceOffset,const void* data,const size_t dataSize){
    void* mappedData = nullptr;
    vkMapMeory(vkDev.device,bufferMemory,deviceOffset,dataSize,0,&mappedData);
    memcpy(mappedData,data,dataSize);
    vkUnmapMemory(vkDev.device,bufferMemory);}
void downloadBufferData(VulkanRenderDevice& vkDev,VkDeviceMemory& bufferMemory,VkDeviceSize deviceOffset,void* outData,const size_t dataSize){
    void* mappedData = nullptr;
    vkMapMemory(vkDev.device,bufferMempry,deviceOffset,dataSize,0,&mappedData);
    memcpy(outData,mappedData,dataSize);
    vkUnmapMemory(vkDev.device,bufferMemory);
}
void downloadBufferData(VulkanRenderDevice& vkDev,VkDeviceMemory& bufferMemory,VkDeviceSize deviceOffset,void* outData,const size_t dataSize){
    void* mappedData = nullptr;
    vkMapMemory(vkDev.device,bufferMemory,deviceOffset,dataSize,0,&mappedData);
    memcpy(outData,mappedData,dataSize);
    vkUnmapMemory(vkDev.device,bufferMemory);
class ComputeBase{
    void uploadInput(uint32_t offset,void* inData,uint32_t byteCount){
        uploadBufferData(vkDev,inBufferMemory,offset,indata,byteCount);}
    void downloadOutput(uint32_t offset,void*outData,uint32_t byteCount){
        downloadBufferData(vkDev,outBufferMemory,offset,outData,byteCount);}
    bool execute(uint32_t xSize,uint32_t ySize,uint32_t zSize){
        return executeComputeShader(vkDev,pipeline,pipelineLayout,descriptorSet,xSize,ySize,zSize);}
ComputeBase::ComputeBase(VulkanRenderDevice& vkDev,const shar* shaderName uint32_t inoutSize,uint32_t outputSize) : vkDev(vkDev){
    createSharedBuffer(vkDev,inputSize,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,inBuffer,inBufferMemory);
    createSharedBuffer(vkDevoutputSize,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,outBuffer,outBufferMemory);
    ShaderModule s;
    createShaderModule(vkDev.device,&s,shaderName);
    createComputeDescriptorSetLayout(vkDev.device,&dsLayout);
    createPipelineLayout(vkDev.device,dsLayout,&pipelineLayout);
    createComputePipeline(vkDev.device,s.shaderModule,pipelineLayout,&pipeline);
    createComputeDescriptorSet(vkDev.device,dsLayout);
    vkDestroyShaderModule(vkDev.device,s.shaderModule,nullptr);}
bool ComputeBase::createComputeDescriptorSet(VkDevice device,VkDescriptorSetLayout descriptorSetLayout){
    VkDescriptorPoolSize descriptorPoolSize = {VK_DESCRIPTOR_TYPE_STORAGE__BUFFER,2};
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {VK_DESCRIPTOR_TYPE_DESCRIPTOR_POOL_CREATE_INFO,0,0,1,1,&descriptorPoolSize};
    VK_CHECK(vkCreateDescriptorPool(device,&descriptorPoolCreateInfo,0,&descriptorPool));
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,0,descriptorPool,1,&descriptorSetLayout};
    VK_CHECK(vkCreateDescriptorPool(device,&descriptorPoolCreateInfo,0,&descriptorPool));
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,0,descriptorPool,1,&descriptorSetLayout};
VK_CHECK(vkAllocateDescriptorSets(device,&descriptorSetAllocateInfo,&descriptorSet));

