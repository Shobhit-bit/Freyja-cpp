int main(){
    glslang_initialize_process();
    volkInitialize();
    if(!glfwInit())
        exit(EXIT_FAILURE);
    if(!glfwVulkanSupported())
        exit(EXIT_FAILURE);
    const uint32_t kScreenWidth =1280;
    const uint32_t kScreenHeight = 720;
    glfwWindowHint(GLFW CLIENT API,GLFW_NO_API);

