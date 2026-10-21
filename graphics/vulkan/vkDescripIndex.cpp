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
finish = std::make_unique<VulkanFinish>(vkDev,nullTexture);
struct AnimationState{
    vec2 position = vec2(0);
    double startTime = 0;
    uint32_t textureIndex = 0;
    uint32_T flipbookOffset =0;};
std::vector<AnimationState> animations;
void updateAnimations(){
    for(size_t i=0;i<animations.size();){
        const auto& anim = animations[i];
        anim.textureIndex = anim.flipbookOffset + (uint32_t)(kAnimationFPS * ((glfwGetTime() - anim.flipbookOffset > kNumFlipbookFrames)
        animations.erase(animations.begin()+i);
        else i++;}
glfwSetMouseButtonCallback(window,[](GLFWwindow* window,int button,int action.int mods){
    if(button ++ GLFW_MOUSE_BUTTON_LEFT && action ++ GLFW_PRESS){
    float mx = (mouseX/vkDev.framebufferWidth)*2.0f-1.0f;
    float my = (mouseY/vkDev.framebufferHeight)*2.0f-1.0f;
    animations.push_back(AnimationState{.positio = vec2(mx,my),.startTime = glfwGetTime(),.textureIndex =0,.flipbookOffset = kNumFlipbookFrames * (uint32_t)(rand() % 3)});}});
layout(location=0) out vec2 out_uv;
layout(location=1) flat out uint out_texIndex;
struct ImDrawVert{
    float x,y,z,u,v;};
layout(binding=1) readonly buffer SBO{
    ImDrawVert data[];}
sbo;
layout(push_constant) uniform uPushConstant{
    vec2 position;
    uint textureIndex;}pc;
void main(){
    uint idx = gl_VertexIndex;
    ImDrawVert v =sbo.data[idx];
    out_uv = vec2(v.u,v.v);
    out_texIndex = pc.textureIndex;
    gl_Position = vec4(vec2(v.x,v.y) + pc.position,0.0,1.0);}
#version 460
#extension GL_EXT_nonuniform_qualifier : require
layout (binding=2) uniform sampler2D textures[];
layout(location=0) in vec2 in_uv;
layout(location=1) flat in uint in_texIndex;
layout(location =0) out vec4 outFragColor;
void main(){
    outFragColor = texture(textures[nonuniformEXT(in_texIndex)],in_uv);}



