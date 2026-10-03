bool createImage(VkDevice device,VkPhysicalDevice physicalDevice,uint32_t width,uint32_t height,VkFormat format,VkImageTiling tiling,VkImageUsageFlags usuage,VkMemoreyPropertyFlags properties,VkImage& image,VkDeviceMemory& imageMemory){
    const VkImageCreateInfo imageInfo = {.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,.pNext=nullptr,.flags=0,.imageType=VK_IMAGE_TYPE_2D,.format=format,.extent=VkExtent3D{.width=width,.height=height,.depth=1},.mipLevels=1,.arrayLayers=1,.samples=VK_SAMPLE_COUNT_1_BIT,.tiling=tiling,.usage=usage,.sharingMode=VK_SHARING_MODE_EXCLUSIVE,.queueFamilyIndexCount=0,.pQueueFamilyIndicies=nullptr,.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED};
    VK_CHECK(vkCrateImage(device,&imageInfo,nullptr,&image));
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device,image,&memRequirements);
    const VkMemoryAllocateInfo ai = {.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,.pNext=nullptr,.allocateSize=memRequirements.size,.memoryTypeIndex = findMemoryType(physicalDevice,memRequirements.memoryTypeBits,properties)};
    VK_CHECK(vkAllocateMemory(device,&ai,nullptr,&imagememory));
    vKBindImageMemory(device,image,imageMemory,0);
    return true;
}
bool createtextureSampler(VkDevice device,VkSampler* sampler){
    comst VkSamplerCreateInfo samplerINfo = {.sType=VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,.pNext=nullptr,.flags=0,.magFilter=VK_FILTER_LINEAR,.minFilter=VK_FILTER_LINEAR,.minmapMode=VK_SAMPLER_MIPMAP_MODE_LINEAR,.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,.affressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,.mipLodBias=0.0f,.anisotorpyEnable = VK_FALSE,.maxAnisotropy = 1,.compareEnable = VK_FALSE,,.compareOp = VK_COMPARE_OP_ALWAYS,.minLod = 0.0f,.maxLod = 0.0f,.borderColor=VK_BORDER_COLOR_INT_OPAQUE_BLACK,.unnormalizedCoordinates = VK_FALSE};
    VK_CHECK(cj=kCreateSampler(device,&samplerInfo,nullptr,sampler));
    return true;}
void copyBufferToImage(VulkanRenderDevice& vkDev,vkBuffer buffer,VkIMage image, uint32_t width,uint32_t height){
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(vkDev);
    const VkBufferImageCopy region = {.bufferOffset =0,.bufferRowLength=0,.bufferImageHeight=0,.imageSubResource=VkImageSubsourceLayers{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,.mipLevel=0,.baseArrayLayer=0,.layerCount=1},.imageOffset=VkOffset3D{.x=0,.y=0,.z=0},.imageExtent = VkExtent3D{.width=width,.height=height,depth=1}
    vkCmdCopyBufferToImage(commandBuffer,buffer,image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&region);
        endSIngleTimeCommands(vkDev,commandBuffer);}
struct VulkanTexture{
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;};
void destroyVulkanTexture(VkDevice device,VulkanTexture& texture){
    vkDestroyImageView(device,texture,.imageView,nullptr);
    vkDestroyImage(device,texture.image,nullotr);
    vkFreeMemory(device,texture.imageMemory,nullptr);}
void translationImageLayout(VulkanRenderDevice& vkDev,VkIMage image,VkFormat format,VkImageLayout oldLayout,VkIMageLayout newLayout,uint32_t layerCount,uint32_t mipLevels){
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(vkDev);
    transitionImageLayoutCmd(commandBuffer,image,format,oldLayout,newLayout,layerCount,mipLevels);
    endSingleTimeCommands(vkDev,commandBuffer);}
void transitionImageLayoutCmd(VkCommandBuffer commandBuffer,VkImage image,VkFormat format,VkImageLayout oldLayout,VkImageLayout newLayout,uint32_t layerCount,uint32_t mipLevels){
    VkImageMemoryBarrier barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,.pNext=nullptr,.srcAccessMask =0,.dstAccessMask=0,.oldLayout=oldLayout,.newLayout=newLayout = newLayout,.srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,.image=image,.subresourceRange=VkImageSubresourcesRange{
        .aspectMask=VK_IMAGE_ASPECT_COLOR_BIT,.baseMipLevel=0,.levelCount=1,.baseArrayLayer=0,.layerCount=1}};
    VkPipelineStageFlags sourceStage,destinationStage;
    if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL){
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if(hasStencilComponents(format))
            barrier.subResourceRange.aspectMask |=VK_IMAGE_ASPECT_STENCIL_BIT;}
    else{ barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;}
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
        barrier.srcAccessMask=0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;}
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL){
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;}
    else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL){
        barrier.srcAccessMask-0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;}
    vkCmdPipelineBarrier(commandBuffer,sourceStage,destinationStage,0,0,nnullptr,0,nullptr,1,&barrier);}
VkFormat findSupprortFormat(VkPhysicalDevice device,const std::vector<VkFormat>&candidates,VkImagesTiling tiling,VkFormatFeatureFlags features){
    const bool isLin = tiling == VK_IMAGE_TILING_LINEAR;
    const bool isOpt = tiling == VK_IMAGE_TILING_OPTIMAL;
    for(VkFormat format : candidates){
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device,format,&props);
        if (isLin && (props.linearTilingFeatures & features) == features)
            return format;
        else if (isOpt && (props.optimaltilingfeatures & features) == features)
            return format;}
    printf("Failed to find supported format!\n");
    exit(0);}
VkFormat findDepthFormat(VkPhysicalDevice device){
    return findSupportedFormat(device,{VK_FORMAT_D32_SFLOAT,VK_FORMAT_D32_SFLOAT_S8_UINT,VK_FORMAT_D24_UNORM_S8_UINT},VK_IMAGE_TILING_OPTIMAL,VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);}
bool hasStencilComponent(VkFormat format){
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;}

