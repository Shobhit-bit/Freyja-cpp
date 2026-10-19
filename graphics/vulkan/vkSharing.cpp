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

