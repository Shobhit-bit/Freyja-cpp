VkPhysicalDeviceDescriptorIndexingFeaturesEXT physicalDeviceDescriptorIndexingFeatures = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE__DESCRIPTOR_INDEXING_FEATURES_EXT,.shaderSampledImageArrayNonUniformIndexing = VK_TRUE,.descriptorBindingVariableDescriptorCount = VK_TRUE,.runtimeDescriptorArray = VK_TRUE,};
const VkPhysicalDeviceFeatures deviceFeatures = {.shaderSampledImageArrayDynamicIndexing = VK_TRUE};
const VkPhysicalDeviceFeatures2 deviceFeatures = {.shaderSampledImageArrayDynamicIndexing = VK_TRUE};
const VkPhysicalDeviceFeatures2 deviceFeatures2 = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,.pNext = &physicalDeviceDescriptorIndexingFeatures,.features = deviceFeatures};
if(!initVulkanRenderDevice2(vk,vkDev,kScreenWidth,kScreenHeight,isDeviceSuitable,deviceFeatures2))
    exit(EXIT_FAILURE);
std::vector<<std::string> textureFiles;
for(uint32_t j=0;j<3;j++){
    for(uint32_t i=0;i!=kNumFlipbookFrames;i++){
        char fname[1024];
        snprintf(fname,sizeof(fname),"data/explosion/explosion%02u-frame%03u.tga",j,i+1);
        textureFiles.push_back(fname);}}
quadRenderer = std::make_unique<VulkanQuadRenderer>(vkDev,textureFiles);
for(size_t i=0;i<vkDev.swapchainImages.size();i++)
fillQuadsBuffer(vkDev,*quadRenderer.get(),i);
VulkanImage nullTexture = {.image = VK_NULL_HANDLE,.imageView = VK_NULL_HANDLE};
clear = std::make_unique<VulkanClear>(vkDev,nullTexture);
finish = std::make_unique<Vull=kanFinish>(vkDev,nullTexture);
struct AnimationState{
    vec2 position = vec2(0);
    double startTime = 0;
    uint32_t textureIndex = 0;
    uint32_T flipbookOffset =0;};
std::vector<AnimationState> animations;

