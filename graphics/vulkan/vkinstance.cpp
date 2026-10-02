static void VK_ASSERT(bool check){
    if (!check) exit(EXIT_FAILURE);}
#define VK_CHECK(value) \ if (Fvalue != VK_SUCCESS) \{VK_ASSERT(false);return false;}
#define VK_CHECK_RET(value) \ if(value != VK_SUCCESS) \ {VK_ASSERT(false);return value;}
void createrInstance(VkInstance* INstance){
    const std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> exts = { "VK_KHR_surface",
#if defined (__linux__)
        "VK_KHR_xcb_surface",
#endif
        VK_EXT_DEBUG_UTILS_EXTENSTION_NAME,VK_EXT_DEBUG_REPORT_EXTENSTION_NAME};
    const VkApplicationInfo appInfo = {.stype = VK_STRUCTURE_TYPE_APPLICATION_INFO,.pnext =nullptr,.pApplicationName = "Vulkan",.applicationVersion = VK_MAKE_VERSION(1,0,0),.pEngineName = "No Engine",.engineVersion = VK_MAKE_VERSION(1,0,0),.apiVersion = VK_API_VERSION_1_1};
    const VkInstanceCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,.pNext = nullptr,.flags=0;.pApplicationInfo = &appInfo,.enabledLayerCount = static_cast<uint32_t>(exts.size()),.ppEnabledExtensionNames = exts.data()};
    VK_ASSERT(vkCreateInstance(&createInfo,nullptr,instance) ==VK_SUCCESS);
    volkLoadInstance(*instance);
}
VkResult createDevice(VkPhysicalDevice physicalDevice,VkPhysicalDeviceFeatures deviceFeatures,uint32_t graphicsFamily,VkDevice* device)
{
const std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
const float queuePriority = 1.0f;
const VkDeviceQueueCreateInfo qci = {.sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,.pNext=nullptr,.flags=0,.queueFamilyIndex = graphicsFamily,.queueCount=1,.pQueuePriorities = &queuePriorirty};
const VkDeviceCreateInfo ci = {.sType = VK_STRUCTURE_TYPE_dEVICE_CREATE_INFO,.pNext=nullptr,.flags=0,.queueCreateInfoCount=1,.pQueueCreateInfos = &qci,.enabledExtensionCount=static_cast<uint2_t>(extensions.size()),.ppEnabledExtensionNames = extensions.data(),.pEnabledFeatures=&deviceFeatures};
return vkCreateDevice(physicalDevice,&ci,nullptr,device);}
VkResult findSuitablePhysicalDevice(
        VkInstance instance,std::function<bool(VkPhysicalDevice)>selector,VkPhysicalDevice* physicalDevice){
    unint32_t deviceCount=0;
    VK_CHECK_RET(vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr));
    if(!devicecount) return VK_ERROR_INITIALIZATION_FAILED;
    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK_RET(vkEnumeratePHysicalDevices(instance,&deviceCount,devices.data()));
    for (const auto& device: devices)
        if (selector(device)){
            *physicalDevice =device;
            return VK_SUCCESS;}
    return VK_ERROR_INITIALIZATION_FAILED;}
unint32_t findQueueFamilies(VKPhysicalDevice device,VkQueueFlags desiredFlags){
    uint32_t familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device,&familyCount,nullptr);
    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device,&familyCount,families.data());
    for(uint32_t i =0;i!=families.size();i++)
        if (families[i].queueCount && (families[i].queueFlags & desiredFlags))
            return i;
    return 0;}
struct SwapchaininSupportDetails{
    VkSurfaceCapabilitiesKHR capabilities = {};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;};
SwapchainSupportDetails querySwapChaininSupport(
        VkPhysicalDevice device,VkSurfaceKHR surface){
    SwapchainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapablitiesKHR(device,surface,&details.capablities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatCount,nullptr);
    if(formatCount){details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatKHR(device,surface,&formatCount,details.formats.data());}
    uint32_t presentModeCnt;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&presentModeCnt,nullptr);
    if(presentModeCnt){details.presentModes.resize(presentModeCnt);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&presentModeCnt,details.presentModes.data());}
    return details;}
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&   availableFormats){
    return{VK_FORMAT_B8G8R8A8_UNORM,VK_COLOR_SPACE_SRGB_    NONLINEAR_KHR};}
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&     availablePresentModes){
    for(const auto mode:availablePresentModes)
        if (mode==VK_PRESENT_MODE_MAILBOX_KHR)  return mode;
    return VK_PRESENT_MODE_FIFO_KHR;
}
uint32_t chooseSwapImageCount(const VkSurfaceCapabilitiesKHR& caps){
    const uint32_t imageCount = caps.minImageCount+1;
    const bool imageCountExceeded = caps.maxImageCount && imageCount > caps.maxImageCount;
    return imageCountExceeded ? caps.maxImageCount : imageCount;}
VkResult createSwapchain(VkDevice device,VkPhysicalDevice physicalDevice,VkSufaceKHRsurface, uint32_t graphicsFamily, uint32_t width,uint32_t height,VkSwapchainKHR* swapchain){
    auto swapchainSupport = querySwapchainSupport(physicalDevice,surface);
    auto surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
    auto presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
    const VkSwapchainCreateInfoKHR ci = {.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,.flags=0,.surface=surface,.minImageCount = chooseSwapImageCount(swapchainSupport.capablities),.imageFormat = surfaceFormat.format,.imageColorSpace=surfaceFormat.colorSpace,.imageExtent = {.width = width,.height=height},.imageArrayLayers=1,.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,.queueFamilyIndexCount =1,.pQueueFamilyIndicies=&graphicsFamily,.preTransform = swapchainSupport.capalities.currentTransform,.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,.presentMode=presentMode,.clipped=VK_TRUE,.oldSwapchain=VK_NULL_HANDLE};
    return vkCreateSwapchainKHR(device,&ci,nullptr,swapchain);}
size_t createSwapchainImages(VkDevice device,VkSwapchainKHR swawpchain,std::vector<VkImage>&swapchainImageViews){
    uint32_t imageCount =0;
    VK_ASSERT(vkGetSwapchainImagesKHR(device,swapchain,&imageCount,nullptr) == VK_SUCCESS);
    swapchainImages.resize(imageCount);
    VK_ASSERT(vkGetSwapchainImagesKHR(device,swapchain,&imageCount,swapchainImages.data()) == VK_SUCCESS);
    for(unsigned i=0;i<imageCount;i++)
        if(!createImageView(device,swapchainImages[i],VK_FORMAT_B8G8R8A8_UNORM,VK_IMAGE_ASPECT_COLOR_BIT,&swapchainImageViews[i]))
            exit(EXIT_FAILURE);
    return imageCount;}
bool createImageView(VkDevice device,VkImage image,VkFormat format,VkImageAspectFlagsaspectFlags,VkImageView* imageView){
    const VkImageViewCreateInfo viewInfo = {.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,.pNext = nullptr,.flags=0,.image=image,.viewType = VK_IMAGE_VIEW_CREATE_INFO,.pNext = nullptr,.flags=0,.image=image,.viewType = VK_IMAGE_VIEW_TYPE_2D,.format=format,.subresourceRange = { .aspectMask =aspectFlags,.baseMipLevel =0,.levelCount=1,.baseArrayLayer = 0,.layerCount=1}};
    VK_CHECK(vkCreateImageView(device,&viewInfo,nullptr,imageView));
    return true;
}
// debuging
static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkanDebugCallback(VkDebugUtilsMessagesSeverityFlagBitsEXT Severity,VkDebugUtilsMessageTypeFlagsEXT Type,const VKDebugUtilsMessengerCallbackDataEXT* CallbackData,void* UserData){
    printf("Validation layer: %s\n",CallbackData->pMessage);
    return VK_FALSE;}
static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkanDebugReportCallback(VkDebugReportFlagsEXT flags,VkDebugReportObjectTypeEXT objectType, uint64_t object,size_t location,int32_t messageCode,const char* pLayerPrefix,const char* pMessage,void* UserData){
    if(flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        return VK_FALSE;
    printf("Debug callback (%s):%s\n",playerPrefix,pMessage);
    return VK_FALSE;}
bool setupDebugCallbacks(VkInstancce instance,VkDebugUtilsMessengerEXT* messenger,VkDebugReportCallbackEXT* reportCallback){
    const VkDebugUtilsMessengerCreateInfoEXT ci1 = {.sType=VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,.messageSeverity=VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,.messageType = VK_DEBUG_UTILS_MESSSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,.pfnUserCallback = &VulkanDebugCallBack,.pUserData = nullptr};
    VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance,&ci1,nullptr,messenger));
    const VkDebugReportCallbackCreateInfoEXT ci2 = { .sType=VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,.pNext = nullptr,.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT,.pNext=nullptr,.flags=VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT,.pfnCallback = &VulkanDebugReportCallback,.pUserData=nullptr};
    VK_CHECK(vkCreateDebugReportCallbackEXT(instance,&ci,nullptr,reportCallback));
    return true;}
//gc
VkResult createSemaphore(VkDevice device,VkSemaphore* outSemaphore){
    const VkSemaphoreCreateInfo ci = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    return vkCreateSeamphore(device,&ci,nullptr,outSemaphore);}
struct VulkanInstance{
    VkInstance instance;
    VkSurfaceKHR suraface;
    VkDebugUtilsMessengerEXT messeger;
    VkDebugReportCallbackEXT reportCallback;};
struct VulkanRenderDevice{
    VkDevice device;
    VkQueue graphicsQueue;
    VkPhysicalDevice physicalDevice;
    uint32_t graphicsFamily;
    VkSemaphore semaphore;
    VkSemaphore renderSemaphore;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;};
bool initVulkanRenderDevice(VulkanInstance& vk,VulkanRenderDevice& vkDev,uint32_t width,uint32_t height,std::function<bool(VkPhysicalDevice)>selector,VkPhysicalDeviceFeatures deviceFeatures){
    VK_CHECK (findSuitablePhysicalDevice(vk.instance,selector,&vkDev.physicalDevice));
    vkDev.graphicsFamily = findQueueFamilies(vkDev.physicalDevice,VK_QUEUE_GRAPHI_BIT);
    VK_CHECK(createDevice(vkDev.physicalDevice,deviceFeatures,vkDev.graphicsFamily,&vkDev.device));
    vkGetDeviceQueue(vkDev.device,vkDev.graphicsFamily,0,&vkDev.graphicsQueue);
    if(vkDev.graphicsQueue == nullptr)  exit(EXIT_FAILURE);
    VkBool32 presentSupported =0;
    vkGetPhysicalDeviceSurfaceSupportKHR(vkDev.physicalDevice,vkDev.graphicsFamily,vkSurface,&presentSupported);
    if(!presentSupported) exit(Exit_FAILURE);
    VK_CHECK(createSwapchain(vkDev.device,vkDev.physicalDevice,vk.surface,vkDev.graphicsFamily,width,height,&vkDev.swapchain));
    const size_t imageCount = createSwapchainImages(vkDev.device,vkDev.swapchain,vkDev.swapchainImages,vkDev.swapchainImageViews);
    vkDev.commandBuffers.resize(imageCount);
    VK_CHECK(createSemaphore(vkDev.device,&vkDev.semaphore));
    VK_CHECK(createSemaphore(vkDev.device,&vkDev.renderSemaphore));
    const VkCommandPoolCreateInfo cpi = {.sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,.flags=0,.queueFamilyIndex=vkDev.graphicsFamily};
    VK_CHECK(vkCreateCommandPool(vkDev.device,&cpi,nullptr,&vkDev.commandPool));
    const VkCommandBufferAllocateInfo ai = {.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,.pNext=nullptr,.commandPool=vkDev.commandPool,.level=VK_COMMAND_BUFFER_LEVEL_PRIMARY,.commandBuffersCount = (uint32_t)(vkDev.swapchainImages.size())};
    VK_CHECK(vkAllocateCommandBuffers(vkDev.device,&ai,&vkDev.commandBuffers[0]));
    return true;}
//deinitialization
void destoryVulkanRenderDevice(VulkanRenderDevice&vkDev){
    for(size_t i=0;i<vkDev.swapchainImages.size();i++)
        vkDestroyImageView(vkDev.device,vkDev.swapchainImageViews[i],nullptr);
    vkDestroySwapchainKHR(vkDev.device,vkDev.swapchain,nullptr);
    vkDestroyCommandPool(vkDev.device,vkDev.swapchain,nullptr);
    vkDestroySemaphore(vkDev.device,vkDev.semaphore,nullptr);
    vkDestroySemaphore(vkDev.device,vkDev.renderSemaphore,nullptr);
    vkDestroyDevice(vkDev.device,nullptr);}
void destroVulkanInstance(VulkanInstance& vk){
    vkDestroySurfaceKHR(vk.instance,vk.surface,nullptr);
    vkDestroyDebugReportCallbackEXT(vk.instance,vk.reportCallback,nullptr);
    vkDestroyDebugUtilsMessengerEXT(vk.instance,vk.messenger,nullptr);
    vkDestroyInstance(vk.instance,mullptr);}
//command buffer
bool fillCommandBuffers(size_t i){
    const VkCommandBufferBEginInfo bi={.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,.pNext=nullptr,.flags=VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,.pInheritanceInfo = nullptr};
    const std::array<VkClearValue,2> clearValues = {VkClearValue{.color=clearValueColor},VkClearValue{.depthStencil={1.0f,0}}};
    const VkRect2D screenRect = {.offset={0,0},.extent={.width=kScreenWidth,.height=kScreenHeight}};
    VK_CHECK(vkBeginCommandBuffer(vkDev.commandBuffer[i],&bi));
    const VkRenderePassBeginInfo renderPassInfo = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,.pNext = nullptr,.renderPass = vkState.renderPass,.frameBuffer=vkState.swapchainFramebuffers[i],.renderArea = screenRect,.clearValueCount = static_cast<uint32_t>(clearValues.size()),.pClearValues=clearValues.data()};
    vkCmdBeginRenderPass(vkDev.commandBuffers[i],&renderPassInfo,VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(vkDev.commandBuffer[i],VK_PIPELINE_BIND_POINT_GRAPHICS,vkState.graphicsPipeline);
    vkCmdBindDescrriptorSets(vkDev.commandBuffers[i],VK_PIPELINE_BIND_POINT_GRAAPHICS,vkState.pipelineLayout,0,1,&vkState.descriptorSets[i],0,nullptr);
    vkCmdDraw(vkDev.commandBUffers[i],static_cast<uint32_t>(indexBufferSize/sizeof(uint32_t)),1,0,0);
    vkCmdEndRenderPass(vkDev.commandBuffer[i]);
    VK_CHECK(vkEndCommandBuffer(vkDev.commandBuffers[i]));
    return true;}

uint32_t findMemoryType(VkPhysicalDevice device,uint32_t typeFilter,VkMemoryPropertyFlags properties){
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device,&memProperties);
    for(uint32_t i=0;i<memProperties.memoryTypeCount;i++){
        if (((typeFilter & (1<<i)) && memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;}
    return 0xFFFFFFFF;}
bool createBuffer(VkDevice device,VkPhysicalDevice physicalDevice,VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags properties,VkBuffer& buffer,VkDeviceMemory& bufferMemory){
    const VkBufferCreateInfo bufferInfo = {.sType=VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,.pNext=nullptr,.flags=0,.size=size,.usage=usage,.sharingMode=VK_SHARING_MODE_EXCLUSIZE,.queueFamilyIndexCount=0,.pQueueFamilyIndicies=nullptr};
    VK_CHECK(vkCreateBuffer(device,&bufferInfo,nullptr,&buffer));
    VkMemoryRequirments memRequirements;
    vkGetBufferMemoryRequirments(device,buffer,&memRequirements);
    const VkMemoryAllocateInfo ai = {.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,.pNext=nullptr,.allocationSize = memRequirements.size,.memoryTypeIndex = findMemoryType(physicaldevice,memRequirments.memoryTypeBits,properties)};
    VK_CHECK(vkAllocateMemory(device,&ai,nullptr,&bufferMemory));
    vkBindBufferMemory(device,buffer,bufferMemory,0);
    return true;
}
void copyBuffer(VkDevice device,VkCommandPool commandPool,VkQueue graphicsQueue,VkBuffer srcBuffer,VkBuffer dstBuffer,VkDeviceSize size){
    VkCommandBuffer commandBuffer=beginSingleTimeCommands(device,commandPool,graphicsQueue);
    const VkBufferCopy copyParam = {.srcOffset=0,.dstOffset=0,.size=size};
    vkCmdCopyBuffer(commandBuffer,srcBuffer,dstBuffer,1,&cpoyParam);
    endSingleTimeCommands(device,commandPool,graphicsQueue,commandBuffer);}
VkCommandBuffer beginSingleTimeCommands(VulkanRenderDevice& vkDev){
    VkCommandBuffer commandBuffer;
    const VkCommandBufferAllocateInfo allocInfo = {.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,.pNext = nullptr,.commandPool = vkDev.commandPool,.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,.commandBufferCount =1};
    vkAllocateCommandBuffers(vkDev.device,&allocInfo,&commandBuffer);
    const VkCommandBufferBeginInfo beginInfo = {.sType =VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,.pNext = nullptr,.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,.pInheritenceInfo = nullptr};
    vkBeginCommandBuffer(commandBuffer,&beginInfo);
    return commandBuffer;}
void endSingleTimeCommands(VulkanRenderDevice& vkDev,VkCommandBuffer commandBuffer){
    vkEndCommandBuffer(commandBuffer);
    const VkSubmitInfo submitInfo = {.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO,.pNext=nullptr,.waitSemaphoreCount=0,.pWaitSemaphores = nullptr,.pWaitDstStageMask = nullptr,.commandBufferCount = 1,.pCommandBuffer = &commandBuffer,.signalSemaphoreCount =0,.pSignalSemaphores=nullptr};
vkQueueSubmit(vkDev.graphicsQueue,1,&submitInfo,VK_NULL_HANDLE);
vkQueueWaitIdle(vkDev.graphicsQueue);
vkFreeCommandBuffers(vkDev.device,vkDev.commandPool,1,&commandBuffer);
}
// init complete 
// uniform buffer 
struct Uniform Buffer{
    mat4 mvp;
} ubo;
bool createUniformBuffers(){
    VkDeviceSize bufferSize = sizeof(UniformBuffer);
    vkState.uniformBuffers.resize(vkDev.swapchainImages.size());
    vkState.uniformBufferMemory.resize(vkDev.swapchainImages.size());
    for(size_t i=0;i<vkDev.swapchainImages.size();i++){
        if(!createBuffer(vkDev.device,vkDev.physicalDevice,bufferSize,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISILE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,vkState.uniformBuffers[i],vkState.uniformBufferMemory[i])){
            printf("Fail: buffers\n");
            return false;}}
    return true;}
void updateUniformBuffer(uint32_t currentImage,const UniformBuffer& ubo){
    void* data = nullptr;
    vkMapMemory(vkDev.device,vkState.uniformBuffersMemory[currentImage],0,sizeof(ubo),0,&data);
    memcpy(data,&ubo,sizeof(ubo));
    vkUnmapMemory(vkDev.device,vkState.uniformBufferMemory[currentImage]);}

