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
