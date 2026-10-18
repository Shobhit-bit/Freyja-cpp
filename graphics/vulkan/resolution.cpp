struct Resolution{
    uint32_t width =0;
    uint32_t height =0;};
GLFWwindow* initVulkanApp(int width,int height,Resolution* outResolution = nullptr){
    glslang_initialize_process();
    volkInitialize();
    if(!glfwInit() || !glfwVulkanSupported())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    if(resolution){
        *resolution = detectResolution(width,height);
        width = resolution->width;
        height= resolution-> height;}
    GLFWwindow* result =glfwCreateWindow(width,height,"VulkanApp",nullptr,nullptr);
    if(!result){
        glfwTerminate();
        exit(EXIT_FAILURE);}
    return result;}
Resolution detectResolution(int width,int height){
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if((glfwGetError(nullptr)) exit(EXIT_FAILURE);
    const GLFWvidmode* info = glfwGetVideoMode(monitor);
    const uint32_t W = width >=0? width : (uint32_t)(info->width * width/-100);
    const uint32_t H = height >= 0 ? height : (uint32_t)(info->height * height/-100);
    return Resolution{.width=W,.height=H};}
bool drawFrame(VulkanRenderDevice& vkDev,const std::function<void(uint32_t)>& updateBuffersFunc,const std::function<void(VkCommandBuffer,uint32_t)>& composeFrameFunc){
    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(vkDev.device,vkDev.swapchain,0,vkDev.semaphore,VK_NULL_HANDLE,&imageIndex);
    if(result!=VK_SUCCESS) return false;
    VK_CHECK(vkResetCommandPool(vkDev.device,vkDev.commandPool,0));
    updateBuffersFunc(imageIndex);
    VkCommandBuffer commandBuffer = vkDev.commandBuffers[imageIndex];
    const VkCommandBufferBeginInfo bi = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,.pNext = nullptr,.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,.pInheritenceInfo = nullptr};
    VK_CHECK(vkBeginCommandBuffer(commandBuffer,&bi));
    composeFrameFunc(commandBuffer,imageINdex);
    VK_CHECK(vkEndCommandBuffer(commandBuffer));
   const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
   const VkSubmitInfo si = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,.pNext = nullptr,.waitSemaphoreCount =1,.pWaitSeamphores = &vkDev.semaphore,.pWaitDstStageMask = waitStages,.commandBufferCount = 1,.pCommandBuffers = &vkDev.commandBuffers[imageIndex],.signalSemaphoreCount = 1,.pSignalSeamphores = &vkDev.renderSemaphore};
   VK_CHECK(vkQueueSubmit(vkDev.graphicsQueue,1,&si,nullptr));
   const VkPresentInfoKHR pi = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,.pNext = nullptr,.waitSemaphoreCount = 1,.pWaitSeamphores = &vkDev.renderSeamphore,.swapchainCount = 1,.pSwapchains = &vkDev.swapchain,.pImageIndices = &imageIndex};
   VK_CHECK(vkQueuePresentKHR(vkDev.graphicsQueue,&pi));
   VK_CHECK(vkDeviceWaitIdle(vkDev.device));
   return true;}

