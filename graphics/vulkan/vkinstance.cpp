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
pg-219
