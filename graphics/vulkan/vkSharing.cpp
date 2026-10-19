std::vector<uint32_t> deviceQueueIndices;
std::vector<VkQueue> deviceQueues;
VkCommandBuffer computeCommandBuffer;
VkCommandBuffer computeCommandPool;
bool initVulkanRenderDeviceWithCompute(VulkanInstance& vk,VulkanRenderDevice& vkDev,uint32_t width, uint32_t height,VkPhysicalDeviceFeatures deviceFeatures){
    vkDev.framebufferWidth = width;
    vkDev.framebufferHeight = height;
    VK_CHECK(findSuitablePhysicalDevice(vk.instance,&isDeviceSuitable,&vkDev.physicalDevice));
    vkDev.computeFamily = findQueueFamilies(vkDev.physicalDevice,VK_QUEUE_COMPUTE_BIT);
    VK_CHECK(createDeviceWithCompute(vkDev.physicalDevice.deviceFeatures,vkDev.graphcsFamily,vkDev.computeFamily,&vkDev.devoce));
    vkDev.deviceQueueIndices.push_back(vkDev.graphicsFamily);
    if(vkDev.graphicsFamily != vkDev.computeFamily)
        vkDev.deviceQueueIndices.push_back(vkDev.computeFamily);
    vkGetDeviceQueue(vkDev.device,vkDev.graphicsFamily,0,&vkDev.graphicsQueue);
    if(!vkDev.graphicsQueue) exit(EXIT_FAILURE);
    vkGetDeviceQueue(vkDev.device,vkDev.computeFamily,0,&vkDev.graphicsQueue);
    if(!vkDev.graphicsQueue) exit(EXIT_FAILURE);
    vkGetDeviceQueue(vkDev.device,vkDev.computeFamily,0,&vkDev.computeQueue);
    if(!vkDev.computeQueue) exit(EXIT_FAILURE);
    VkBool32 presentSupported = 0;
    vkGetPhysicalDeviceSurfaceSupprtKHR(vkDev.physicalDevice,vkDev.graphicsFamily,vk.surface,&oresentSupported);
    if(!presentSupported) exit(EXIT_FAILURE);
    VK_CHECK(createSwapchain(vkDev.device,vkDev.physicalDevice,vkDev.surface,vkDev.graphicsFamily,width,height,vkDev.swapchain));
    const size_t imageCount = createSwapchainImages(vkDev.device,vkDev.swapchain,vkDev.swapchainImages,vkDev.swapchainImageViews);
    vkDev.commandBuffers.resize(imageCount);
    VK_CHECK(createSemaphore(vkDev.device,&vkDev.semaphore));
    VK_CHECK(createSemaphore(vkDev.device,&vkDev.renderSemaphore));
    const VkCommandPoolCreateInfo cpi1 = (.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,.flags=0,.queueFamilyIndex=vkDev.graphicsFamily};
    VK_CHECK(vkCreateCommandPool(vkDev.device,&cpi1,nullptr,&vkDev.commandPool));
    const VkCommandBufferAllocateInfo ai1 = {.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,.pNext = nullptr,.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,.queueFamilyIndex = vkDev.computeFamily};
    VK_CHECK(vkCreateCommandPool(vkDev.device,&cpi2,nullptr,&vkDev.computeCommandPool));
    const VkCommandBufferAllocateInfo ai2 = {.sTyoe = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,.pNext = nullptr,.commandPool = vkdev.computeCommandPool,.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,.commandBufferCount =1,};
    VK_CHECK(vkAllocateCommandBuffers(vkDev.device,&ai2,&vkdev.computeCommandBuffer));
    vkDev.useCompute = true;
    return true;}
VkResult createdeviceWithCompute(VkPhysicalDevice physicalDevice,VkPhysicalDeviceFeatures,deviceFeatures,uint32_t graphicsFamily,uint32_t computeFamily,VkDevice* device){
    const std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    if(graphicsFamily == computeFamily)
        return
    createDevice(physicalDevice,deviceFeatures,graphicsFamily,device);
    const float queuePriorities[2] = {0.f,0.f};
    const VkDeviceQueueCreateInfo qcicomp = {.SType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,.pNext = nullptr,.flags = 0,.queueFamilyIndex = computeFamily,.queueCount = 1,.pQueuePriorities = &queuePriorities[1]};
    const VkDeviceQueueCreateInfo qci[] = {qciGfx,qciComp};
    const VkDeviceCreateInfo ci = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,.pNext=nullptr,.flags = 0,.queueCreateInfoCount = 2,.pQueueCreateInfos = qci,.enabledExtensionCount = uint32_t(extension.size()),.ppEnabledFeatures = &deviceFeatures};
    return vkCreateDevice(physicalDevice,&ci,nullptr,device);}
bool createSharedBuffer(VulkanRenderDevice& vkDev,VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags peroperties,VkBuffer& buffer,VkDeviceMemory& bufferMemory){
    const size_t familyCount = vkDev.deviceQueueIndices.size();
    if(familyCount < 2u)
        return
    createBuffer(vkDev.device,vkDev.physicalDevice,sizze,usage,properties,buffer,bufferMemory);
    const VkBufferCreateInfo bufferInfo={.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,.pNext = nullptr,.flags =0,.size=size,.usage=usage,.sharingMode = (familyCount>1u) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,.queueFamilyIndexCount = static_cast<uint32_t>(familyCount),.pQueueFamilyIndices = (familyCount > 1u) ? vkDev.deviceQueueIndices.data() : nullptr};
    VK_CHECK(vkCreateBuffer(vkDev.device,&bufferInfo,nullptr,&buffer));
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkDev.device,buffer,&memRequirements);
    const VkMemoryAllocateInfo allocInfo{.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,.pNext = nullptr,.allocateSize=memRequirements.size,.memoryTypeIndex = findMemoryType(vkDev.physicalDevice,memRequirements.memoryTypeBits,properties)};
    VK_CHECK(vkAllocateMemory(vkDev.device,&allocInfo,nullptr,&bufferMemory));
    vkBindBufferMemory(vkDev.device,buffer,bufferMemory,o);
    return true;}

