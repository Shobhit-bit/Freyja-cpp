bool createImage(VkDevice device,VkPhysicalDevice physicalDevice,uint32_t width,uint32_t height,VkFormat format,VkImageTiling tiling,VkImageUsageFlags usage,VkMemoryPropertyFlags properties,VkImage& image,VkDeviceMemory& imageMemory){
    const VkImageCreateInfo imageInfo = {.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,.pNext=nullptr,.flags=0,.imageType=VK_IMAGE_TYPE_2D,.format=format,.extent=VkExtent3D{.width=width,.height=height,.depth=1},.mipLevels=1,.arrayLayers=1,.samples=VK_SAMPLE_COUNT_1_BIT,.tiling=tiling,.usage=usage,.sharingMode=VK_SHARING_MODE_EXCLUSIVE,.queueFamilyIndexCount=0,.pQueueFamilyIndices=nullptr,.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED};
    VK_CHECK(vkCreateImage(device,&imageInfo,nullptr,&image));
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device,image,&memRequirements);
    const VkMemoryAllocateInfo ai = {.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,.pNext=nullptr,.allocationSize=memRequirements.size,.memoryTypeIndex = findMemoryType(physicalDevice,memRequirements.memoryTypeBits,properties)};
    VK_CHECK(vkAllocateMemory(device,&ai,nullptr,&imageMemory));
    VK_CHECK(vkBindImageMemory(device,image,imageMemory,0));
    return true;
}
bool createTextureSampler(VkDevice device,VkSampler* sampler){
    const VkSamplerCreateInfo samplerInfo = {.sType=VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,.pNext=nullptr,.flags=0,.magFilter=VK_FILTER_LINEAR,.minFilter=VK_FILTER_LINEAR,.mipmapMode=VK_SAMPLER_MIPMAP_MODE_LINEAR,.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,.mipLodBias=0.0f,.anisotropyEnable = VK_FALSE,.maxAnisotropy = 1,.compareEnable = VK_FALSE,.compareOp = VK_COMPARE_OP_ALWAYS,.minLod = 0.0f,.maxLod = 0.0f,.borderColor=VK_BORDER_COLOR_INT_OPAQUE_BLACK,.unnormalizedCoordinates = VK_FALSE};
    VK_CHECK(vkCreateSampler(device,&samplerInfo,nullptr,sampler));
    return true;}
void copyBufferToImage(VulkanRenderDevice& vkDev,VkBuffer buffer,VkImage image, uint32_t width,uint32_t height){
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(vkDev);
    const VkBufferImageCopy region = {.bufferOffset =0,.bufferRowLength=0,.bufferImageHeight=0,.imageSubresource=VkImageSubresourceLayers{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,.mipLevel=0,.baseArrayLayer=0,.layerCount=1},.imageOffset=VkOffset3D{.x=0,.y=0,.z=0},.imageExtent = VkExtent3D{.width=width,.height=height,.depth=1}};
    vkCmdCopyBufferToImage(commandBuffer,buffer,image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&region);
    endSingleTimeCommands(vkDev,commandBuffer);}
struct VulkanImage{
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;};
void destroyVulkanImage(VkDevice device,VulkanImage& texture){
    vkDestroyImageView(device,texture.imageView,nullptr);
    vkDestroyImage(device,texture.image,nullptr);
    vkFreeMemory(device,texture.imageMemory,nullptr);}
bool hasStencilComponent(VkFormat format){
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;}
void transitionImageLayoutCmd(VkCommandBuffer commandBuffer,VkImage image,VkFormat format,VkImageLayout oldLayout,VkImageLayout newLayout,uint32_t layerCount,uint32_t mipLevels){
    VkImageMemoryBarrier barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,.pNext=nullptr,.srcAccessMask =0,.dstAccessMask=0,.oldLayout=oldLayout,.newLayout=newLayout,.srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,.image=image,.subresourceRange=VkImageSubresourceRange{
        .aspectMask=VK_IMAGE_ASPECT_COLOR_BIT,.baseMipLevel=0,.levelCount=mipLevels,.baseArrayLayer=0,.layerCount=layerCount}};
    VkPipelineStageFlags sourceStage,destinationStage;
    if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL){
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if(hasStencilComponent(format))
            barrier.subresourceRange.aspectMask |=VK_IMAGE_ASPECT_STENCIL_BIT;}
    else{ barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;}
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
        barrier.srcAccessMask=0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;}
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL){
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;}
    else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL){
        barrier.srcAccessMask=0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;}
    vkCmdPipelineBarrier(commandBuffer,sourceStage,destinationStage,0,0,nullptr,0,nullptr,1,&barrier);}
void transitionImageLayout(VulkanRenderDevice& vkDev,VkImage image,VkFormat format,VkImageLayout oldLayout,VkImageLayout newLayout,uint32_t layerCount = 1,uint32_t mipLevels = 1){
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(vkDev);
    transitionImageLayoutCmd(commandBuffer,image,format,oldLayout,newLayout,layerCount,mipLevels);
    endSingleTimeCommands(vkDev,commandBuffer);}
VkFormat findSupportedFormat(VkPhysicalDevice device,const std::vector<VkFormat>&candidates,VkImageTiling tiling,VkFormatFeatureFlags features){
    const bool isLin = tiling == VK_IMAGE_TILING_LINEAR;
    const bool isOpt = tiling == VK_IMAGE_TILING_OPTIMAL;
    for(VkFormat format : candidates){
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device,format,&props);
        if (isLin && (props.linearTilingFeatures & features) == features)
            return format;
        else if (isOpt && (props.optimalTilingFeatures & features) == features)
            return format;}
    printf("Failed to find supported format!\n");
    exit(0);}
VkFormat findDepthFormat(VkPhysicalDevice device){
    return findSupportedFormat(device,{VK_FORMAT_D32_SFLOAT,VK_FORMAT_D32_SFLOAT_S8_UINT,VK_FORMAT_D24_UNORM_S8_UINT},VK_IMAGE_TILING_OPTIMAL,VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);}
void createDepthResources(VulkanRenderDevice& vkDev,uint32_t width,uint32_t height,VulkanImage& depth){
    VkFormat depthFormat = findDepthFormat(vkDev.physicalDevice);
    createImage(vkDev.device,vkDev.physicalDevice,width,height,depthFormat,VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,depth.image,depth.imageMemory);
    createImageView(vkDev.device,depth.image,depthFormat,VK_IMAGE_ASPECT_DEPTH_BIT,&depth.imageView);
    transitionImageLayout(vkDev,depth.image,depthFormat,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);}
bool createTextureImage(VulkanRenderDevice& vkDev,const char* filename,VkImage& textureImage,VkDeviceMemory& textureImageMemory){
    int texWidth,texHeight,texChannels;
    stbi_uc* pixels= stbi_load(filename,&texWidth,&texHeight,&texChannels,STBI_rgb_alpha);
    if(!pixels){
        printf("Failed to load [%s] texture\n",filename);
        return false;}
    VkDeviceSize imageSize = (VkDeviceSize)texWidth * texHeight * 4;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    createBuffer(vkDev.device,vkDev.physicalDevice,imageSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,stagingBuffer,stagingMemory);
    void* data;
    vkMapMemory(vkDev.device,stagingMemory,0,imageSize,0,&data);
    memcpy(data,pixels,static_cast<size_t>(imageSize));
    vkUnmapMemory(vkDev.device,stagingMemory);
    createImage(vkDev.device,vkDev.physicalDevice,texWidth,texHeight,VK_FORMAT_R8G8B8A8_UNORM,VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,textureImage,textureImageMemory);
    transitionImageLayout(vkDev,textureImage,VK_FORMAT_R8G8B8A8_UNORM,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(vkDev,stagingBuffer,textureImage,static_cast<uint32_t>(texWidth),static_cast<uint32_t>(texHeight));
    transitionImageLayout(vkDev,textureImage,VK_FORMAT_R8G8B8A8_UNORM,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    vkDestroyBuffer(vkDev.device,stagingBuffer,nullptr);
    vkFreeMemory(vkDev.device,stagingMemory,nullptr);
    stbi_image_free(pixels);
    return true;}
