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
    vkGetPhysical

