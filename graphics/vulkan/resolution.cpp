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
        height=
