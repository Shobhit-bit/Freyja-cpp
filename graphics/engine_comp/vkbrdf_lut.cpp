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

    
