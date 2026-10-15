static void VK_ASSERT(bool check){
    if (!check) exit(EXIT_FAILURE);}
#define VK_CHECK(value) do{ if((value) != VK_SUCCESS){ VK_ASSERT(false);} }while(0)
#define VK_CHECK_RET(value) do{ const VkResult vkr_ = (value); if(vkr_ != VK_SUCCESS){ VK_ASSERT(false); return vkr_;} }while(0)
void createInstance(VkInstance* instance){
    VK_ASSERT(volkInitialize() == VK_SUCCESS);
    const std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> exts = { "VK_KHR_surface",
#if defined (_WIN32)
        "VK_KHR_win32_surface",
#endif
#if defined (__linux__)
        "VK_KHR_xcb_surface",
#endif
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
    const VkApplicationInfo appInfo = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,.pNext =nullptr,.pApplicationName = "Vulkan",.applicationVersion = VK_MAKE_VERSION(1,0,0),.pEngineName = "No Engine",.engineVersion = VK_MAKE_VERSION(1,0,0),.apiVersion = VK_API_VERSION_1_1};
    const VkInstanceCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,.pNext = nullptr,.flags=0,.pApplicationInfo = &appInfo,.enabledLayerCount = static_cast<uint32_t>(layers.size()),.ppEnabledLayerNames = layers.data(),.enabledExtensionCount = static_cast<uint32_t>(exts.size()),.ppEnabledExtensionNames = exts.data()};
    VK_ASSERT(vkCreateInstance(&createInfo,nullptr,instance) ==VK_SUCCESS);
    volkLoadInstance(*instance);
}
VkResult createDevice(VkPhysicalDevice physicalDevice,VkPhysicalDeviceFeatures deviceFeatures,uint32_t graphicsFamily,VkDevice* device)
{
const std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
const float queuePriority = 1.0f;
const VkDeviceQueueCreateInfo qci = {.sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,.pNext=nullptr,.flags=0,.queueFamilyIndex = graphicsFamily,.queueCount=1,.pQueuePriorities = &queuePriority};
const VkDeviceCreateInfo ci = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,.pNext=nullptr,.flags=0,.queueCreateInfoCount=1,.pQueueCreateInfos = &qci,.enabledExtensionCount=static_cast<uint32_t>(extensions.size()),.ppEnabledExtensionNames = extensions.data(),.pEnabledFeatures=&deviceFeatures};
return vkCreateDevice(physicalDevice,&ci,nullptr,device);}
VkResult findSuitablePhysicalDevice(
        VkInstance instance,std::function<bool(VkPhysicalDevice)>selector,VkPhysicalDevice* physicalDevice){
    uint32_t deviceCount=0;
    VK_CHECK_RET(vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr));
    if(!deviceCount) return VK_ERROR_INITIALIZATION_FAILED;
    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK_RET(vkEnumeratePhysicalDevices(instance,&deviceCount,devices.data()));
    for (const auto& device: devices)
        if (selector(device)){
            *physicalDevice =device;
            return VK_SUCCESS;}
    return VK_ERROR_INITIALIZATION_FAILED;}
uint32_t findQueueFamilies(VkPhysicalDevice device,VkQueueFlags desiredFlags){
    uint32_t familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device,&familyCount,nullptr);
    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device,&familyCount,families.data());
    for(uint32_t i =0;i!=families.size();i++)
        if (families[i].queueCount && (families[i].queueFlags & desiredFlags))
            return i;
    return 0;}
struct SwapchainSupportDetails{
    VkSurfaceCapabilitiesKHR capabilities = {};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;};
SwapchainSupportDetails querySwapchainSupport(
        VkPhysicalDevice device,VkSurfaceKHR surface){
    SwapchainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface,&details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatCount,nullptr);
    if(formatCount){details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatCount,details.formats.data());}
    uint32_t presentModeCnt;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&presentModeCnt,nullptr);
    if(presentModeCnt){details.presentModes.resize(presentModeCnt);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&presentModeCnt,details.presentModes.data());}
    return details;}
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&   availableFormats){
    return{VK_FORMAT_B8G8R8A8_UNORM,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};}
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&     availablePresentModes){
    for(const auto mode:availablePresentModes)
        if (mode==VK_PRESENT_MODE_MAILBOX_KHR)  return mode;
    return VK_PRESENT_MODE_FIFO_KHR;
}
uint32_t chooseSwapImageCount(const VkSurfaceCapabilitiesKHR& caps){
    const uint32_t imageCount = caps.minImageCount+1;
    const bool imageCountExceeded = caps.maxImageCount && imageCount > caps.maxImageCount;
    return imageCountExceeded ? caps.maxImageCount : imageCount;}
VkResult createSwapchain(VkDevice device,VkPhysicalDevice physicalDevice,VkSurfaceKHR surface, uint32_t graphicsFamily, uint32_t width,uint32_t height,VkSwapchainKHR* swapchain){
    auto swapchainSupport = querySwapchainSupport(physicalDevice,surface);
    auto surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
    auto presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
    const VkSwapchainCreateInfoKHR ci = {.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,.flags=0,.surface=surface,.minImageCount = chooseSwapImageCount(swapchainSupport.capabilities),.imageFormat = surfaceFormat.format,.imageColorSpace=surfaceFormat.colorSpace,.imageExtent = {.width = width,.height=height},.imageArrayLayers=1,.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,.queueFamilyIndexCount =1,.pQueueFamilyIndices=&graphicsFamily,.preTransform = swapchainSupport.capabilities.currentTransform,.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,.presentMode=presentMode,.clipped=VK_TRUE,.oldSwapchain=VK_NULL_HANDLE};
    return vkCreateSwapchainKHR(device,&ci,nullptr,swapchain);}
bool createImageView(VkDevice device,VkImage image,VkFormat format,VkImageAspectFlags aspectFlags,VkImageView* imageView,VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D,uint32_t layerCount = 1){
    const VkImageViewCreateInfo viewInfo = {.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,.pNext = nullptr,.flags=0,.image=image,.viewType = viewType,.format=format,.subresourceRange = { .aspectMask =aspectFlags,.baseMipLevel =0,.levelCount=1,.baseArrayLayer = 0,.layerCount=layerCount}};
    VK_CHECK(vkCreateImageView(device,&viewInfo,nullptr,imageView));
    return true;
}
size_t createSwapchainImages(VkDevice device,VkSwapchainKHR swapchain,std::vector<VkImage>& swapchainImages,std::vector<VkImageView>& swapchainImageViews){
    uint32_t imageCount =0;
    VK_ASSERT(vkGetSwapchainImagesKHR(device,swapchain,&imageCount,nullptr) == VK_SUCCESS);
    swapchainImages.resize(imageCount);
    swapchainImageViews.resize(imageCount);
    VK_ASSERT(vkGetSwapchainImagesKHR(device,swapchain,&imageCount,swapchainImages.data()) == VK_SUCCESS);
    for(unsigned i=0;i<imageCount;i++)
        if(!createImageView(device,swapchainImages[i],VK_FORMAT_B8G8R8A8_UNORM,VK_IMAGE_ASPECT_COLOR_BIT,&swapchainImageViews[i]))
            exit(EXIT_FAILURE);
    return imageCount;}
// debuging
static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT Severity,VkDebugUtilsMessageTypeFlagsEXT Type,const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,void* UserData){
    printf("Validation layer: %s\n",CallbackData->pMessage);
    return VK_FALSE;}
static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkanDebugReportCallback(VkDebugReportFlagsEXT flags,VkDebugReportObjectTypeEXT objectType, uint64_t object,size_t location,int32_t messageCode,const char* pLayerPrefix,const char* pMessage,void* UserData){
    if(flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        return VK_FALSE;
    printf("Debug callback (%s):%s\n",pLayerPrefix,pMessage);
    return VK_FALSE;}
bool setupDebugCallbacks(VkInstance instance,VkDebugUtilsMessengerEXT* messenger,VkDebugReportCallbackEXT* reportCallback){
    const VkDebugUtilsMessengerCreateInfoEXT ci1 = {.sType=VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,.messageSeverity=VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,.pfnUserCallback = &vulkanDebugCallback,.pUserData = nullptr};
    VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance,&ci1,nullptr,messenger));
    const VkDebugReportCallbackCreateInfoEXT ci2 = { .sType=VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,.pNext=nullptr,.flags=VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT,.pfnCallback = &vulkanDebugReportCallback,.pUserData=nullptr};
    VK_CHECK(vkCreateDebugReportCallbackEXT(instance,&ci2,nullptr,reportCallback));
    return true;}
//gc
VkResult createSemaphore(VkDevice device,VkSemaphore* outSemaphore){
    const VkSemaphoreCreateInfo ci = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    return vkCreateSemaphore(device,&ci,nullptr,outSemaphore);}
struct VulkanInstance{
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT messenger;
    VkDebugReportCallbackEXT reportCallback;};
struct VulkanRenderDevice{
    VkDevice device;
    VkQueue graphicsQueue;
    VkPhysicalDevice physicalDevice;
    uint32_t graphicsFamily;
    uint32_t framebufferWidth;
    uint32_t framebufferHeight;
    VkSemaphore semaphore;
    VkSemaphore renderSemaphore;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;};
bool initVulkanRenderDevice(VulkanInstance& vk,VulkanRenderDevice& vkDev,uint32_t width,uint32_t height,std::function<bool(VkPhysicalDevice)>selector,VkPhysicalDeviceFeatures deviceFeatures){
    vkDev.framebufferWidth = width;
    vkDev.framebufferHeight = height;
    VK_CHECK (findSuitablePhysicalDevice(vk.instance,selector,&vkDev.physicalDevice));
    vkDev.graphicsFamily = findQueueFamilies(vkDev.physicalDevice,VK_QUEUE_GRAPHICS_BIT);
    VK_CHECK(createDevice(vkDev.physicalDevice,deviceFeatures,vkDev.graphicsFamily,&vkDev.device));
    vkGetDeviceQueue(vkDev.device,vkDev.graphicsFamily,0,&vkDev.graphicsQueue);
    if(vkDev.graphicsQueue == nullptr)  exit(EXIT_FAILURE);
    VkBool32 presentSupported =0;
    vkGetPhysicalDeviceSurfaceSupportKHR(vkDev.physicalDevice,vkDev.graphicsFamily,vk.surface,&presentSupported);
    if(!presentSupported) exit(EXIT_FAILURE);
    VK_CHECK(createSwapchain(vkDev.device,vkDev.physicalDevice,vk.surface,vkDev.graphicsFamily,width,height,&vkDev.swapchain));
    const size_t imageCount = createSwapchainImages(vkDev.device,vkDev.swapchain,vkDev.swapchainImages,vkDev.swapchainImageViews);
    vkDev.commandBuffers.resize(imageCount);
    VK_CHECK(createSemaphore(vkDev.device,&vkDev.semaphore));
    VK_CHECK(createSemaphore(vkDev.device,&vkDev.renderSemaphore));
    const VkCommandPoolCreateInfo cpi = {.sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,.flags=0,.queueFamilyIndex=vkDev.graphicsFamily};
    VK_CHECK(vkCreateCommandPool(vkDev.device,&cpi,nullptr,&vkDev.commandPool));
    const VkCommandBufferAllocateInfo ai = {.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,.pNext=nullptr,.commandPool=vkDev.commandPool,.level=VK_COMMAND_BUFFER_LEVEL_PRIMARY,.commandBufferCount = (uint32_t)(vkDev.swapchainImages.size())};
    VK_CHECK(vkAllocateCommandBuffers(vkDev.device,&ai,&vkDev.commandBuffers[0]));
    return true;}
//deinitialization
void destroyVulkanRenderDevice(VulkanRenderDevice&vkDev){
    for(size_t i=0;i<vkDev.swapchainImages.size();i++)
        vkDestroyImageView(vkDev.device,vkDev.swapchainImageViews[i],nullptr);
    vkDestroySwapchainKHR(vkDev.device,vkDev.swapchain,nullptr);
    vkDestroyCommandPool(vkDev.device,vkDev.commandPool,nullptr);
    vkDestroySemaphore(vkDev.device,vkDev.semaphore,nullptr);
    vkDestroySemaphore(vkDev.device,vkDev.renderSemaphore,nullptr);
    vkDestroyDevice(vkDev.device,nullptr);}
void destroyVulkanInstance(VulkanInstance& vk){
    vkDestroySurfaceKHR(vk.instance,vk.surface,nullptr);
    vkDestroyDebugReportCallbackEXT(vk.instance,vk.reportCallback,nullptr);
    vkDestroyDebugUtilsMessengerEXT(vk.instance,vk.messenger,nullptr);
    vkDestroyInstance(vk.instance,nullptr);}
// (old fillCommandBuffers() removed: it used global vkState and is replaced by the Renderer classes)
uint32_t findMemoryType(VkPhysicalDevice device,uint32_t typeFilter,VkMemoryPropertyFlags properties){
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device,&memProperties);
    for(uint32_t i=0;i<memProperties.memoryTypeCount;i++){
        if ((typeFilter & (1<<i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;}
    return 0xFFFFFFFF;}
bool createBuffer(VkDevice device,VkPhysicalDevice physicalDevice,VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags properties,VkBuffer& buffer,VkDeviceMemory& bufferMemory){
    const VkBufferCreateInfo bufferInfo = {.sType=VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,.pNext=nullptr,.flags=0,.size=size,.usage=usage,.sharingMode=VK_SHARING_MODE_EXCLUSIVE,.queueFamilyIndexCount=0,.pQueueFamilyIndices=nullptr};
    VK_CHECK(vkCreateBuffer(device,&bufferInfo,nullptr,&buffer));
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device,buffer,&memRequirements);
    const VkMemoryAllocateInfo ai = {.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,.pNext=nullptr,.allocationSize = memRequirements.size,.memoryTypeIndex = findMemoryType(physicalDevice,memRequirements.memoryTypeBits,properties)};
    VK_CHECK(vkAllocateMemory(device,&ai,nullptr,&bufferMemory));
    vkBindBufferMemory(device,buffer,bufferMemory,0);
    return true;
}
VkCommandBuffer beginSingleTimeCommands(VulkanRenderDevice& vkDev){
    VkCommandBuffer commandBuffer;
    const VkCommandBufferAllocateInfo allocInfo = {.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,.pNext = nullptr,.commandPool = vkDev.commandPool,.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,.commandBufferCount =1};
    vkAllocateCommandBuffers(vkDev.device,&allocInfo,&commandBuffer);
    const VkCommandBufferBeginInfo beginInfo = {.sType =VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,.pNext = nullptr,.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,.pInheritanceInfo = nullptr};
    vkBeginCommandBuffer(commandBuffer,&beginInfo);
    return commandBuffer;}
void endSingleTimeCommands(VulkanRenderDevice& vkDev,VkCommandBuffer commandBuffer){
    vkEndCommandBuffer(commandBuffer);
    const VkSubmitInfo submitInfo = {.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO,.pNext=nullptr,.waitSemaphoreCount=0,.pWaitSemaphores = nullptr,.pWaitDstStageMask = nullptr,.commandBufferCount = 1,.pCommandBuffers = &commandBuffer,.signalSemaphoreCount =0,.pSignalSemaphores=nullptr};
    vkQueueSubmit(vkDev.graphicsQueue,1,&submitInfo,VK_NULL_HANDLE);
    vkQueueWaitIdle(vkDev.graphicsQueue);
    vkFreeCommandBuffers(vkDev.device,vkDev.commandPool,1,&commandBuffer);
}
void copyBuffer(VulkanRenderDevice& vkDev,VkBuffer srcBuffer,VkBuffer dstBuffer,VkDeviceSize size){
    VkCommandBuffer commandBuffer=beginSingleTimeCommands(vkDev);
    const VkBufferCopy copyParam = {.srcOffset=0,.dstOffset=0,.size=size};
    vkCmdCopyBuffer(commandBuffer,srcBuffer,dstBuffer,1,&copyParam);
    endSingleTimeCommands(vkDev,commandBuffer);}
// init complete
// uniform buffer helpers used by RendererBase and the renderers
bool createUniformBuffer(VulkanRenderDevice& vkDev,VkBuffer& buffer,VkDeviceMemory& bufferMemory,VkDeviceSize bufferSize){
    return createBuffer(vkDev.device,vkDev.physicalDevice,bufferSize,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,buffer,bufferMemory);}
void uploadBufferData(VulkanRenderDevice& vkDev,const VkDeviceMemory& bufferMemory,VkDeviceSize deviceOffset,const void* data,const size_t dataSize){
    void* mappedData = nullptr;
    vkMapMemory(vkDev.device,bufferMemory,deviceOffset,dataSize,0,&mappedData);
    memcpy(mappedData,data,dataSize);
    vkUnmapMemory(vkDev.device,bufferMemory);}
