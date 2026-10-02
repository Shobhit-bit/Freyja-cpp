bool createImage(VkDevice device,VkPhysicalDevice physicalDevice,uint32_t width,uint32_t height,VkFormat format,VkImageTiling tiling,VkImageUsageFlags usuage,VkMemoreyPropertyFlags properties,VkImage& image,VkDeviceMemory& imageMemory){
    const VkImageCreateInfo imageInfo = {.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,.pNext=nullptr,.flags=0,.imageType=VK_IMAGE_TYPE_2D,.format=format,.extent=VkExtent3D{.width=width,.height=height,.depth=1},.mipLevels=1,.arrayLayers=1,.samples=VK_SAMPLE_COUNT_1_BIT,.tiling=tiling,.usage=usage,.sharingMode=VK_SHARING_MODE_EXCLUSIVE,.queueFamilyIndexCount=0,.pQueueFamilyIndicies=nullptr,.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED};
    VK_CHECK(vkCrateImage(device,&imageInfo,nullptr,&image));
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device,image,&memRequirements);
    const VkMemoryAllocateInfo ai = {.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,.pNext=nullptr,.allocateSize=memRequirements.size,.memoryTypeIndex = findMemoryType(physicalDevice,memRequirements
