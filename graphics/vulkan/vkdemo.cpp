int main(){
    glslang_initialize_process();
    volkInitialize();
    if(!glfwInit())
        exit(EXIT_FAILURE);
    if(!glfwVulkanSupported())
        exit(EXIT_FAILURE);
    const uint32_t kScreenWidth =1280;
    const uint32_t kScreenHeight = 720;
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    window = glfwCreateWindow(kScreenWidth,ScreenHeight,"VulkanApp",nullptr,nullptr);
initVulkan();
while(!glfwWindowShouldClose(window)){
    drawOverlay();
    glfwPollEvents();}
terminateVulkan();
glfwTerminate();
glslang_finalize_process();
return 0;}
bool initVulkan(){
    createInstance(&vk.instance);
    if(!setupDebugCallbacks(vk.instance,&vk.messenger,&vk.reportCallback))
        exit(EXIT_FAILURE);
    if(glfwCreateWindowSurface(vk.instance,window,nullptr,&vk.surface))
        exit(EXIT_FAILURE);
    if(!inntVulkanRenderDevice(vk,vkDev,kScreenWidth,kScreenHeight,isDeviceSuitable,{.geometryShader=VK_TRUE}))
        exit(EXIT_FAILURE);
    VK_CHECK(createShaderMOdule(vkDev.device,&vkState.vertShader,"data/shaders/VK02.vert"));
    VK_CHECK(createShaderModule(vkDev.device,&vkState.fragShader,"data/shader/VK02.frag"));
    VK_CHECK(createShaderMPdule(vkDev.device,&vkState.geomShader,"data/shader/VK02.geom"));
    if(!createTextured
        
