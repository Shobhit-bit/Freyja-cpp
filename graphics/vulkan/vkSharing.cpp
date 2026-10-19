std::vector<uint32_t> deviceQueueIndices;
std::vector<VkQueue> deviceQueues;
VkCommandBuffer computeCommandBuffer;
VkCommandBuffer computeCommandPool;
bool initVulkanRenderDeviceWithCompute(VulkanInstance& vk,VulkanRenderDevice& vkDev,uint32_t width, uint32_t height,VkPhysicalDeviceFeatures deviceFeatures){
    vkDev.framebufferWidth = width;
    vkDev.framebufferHeight = height;
    VK_CHECK(findSuitablePhysicalDevice(vk.instance,&isDeviceSuitable,&vkDev.physicalDevice));
    vkDev.computeFamily = findQueueFamilies(vkDev.physicalDevice,VK_QUEUE_COMPUTE_BIT);
    VK_CHECK(
