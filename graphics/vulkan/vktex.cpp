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

