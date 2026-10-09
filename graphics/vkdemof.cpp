VulkanInstance vk;
VulkanRenderDevice vkDev;
std::unique_ptr<ImGuiRenderer> imgui;
std::unique_ptr<ModelRenderer> modelRenderer;
std::unique_ptr<CubeRenderer> cubeRenderer;
std::unique_ptr<VulkanCanvas> canvas;
std::unique_ptr<VulkanCanvas> canvas2d;
std::unique_ptr<VulkanClear> clear;
std::unique_ptr<VulkanFinish> finish;
FramesPerSecondCounter fpsCounter(0.02f);
LinearGraph fpsGraph;
LinearGraph sineGraph(4096);
glm::vec3 cameraPos(0.0f,0.0f,0.0f);
glm::vec3 cameraAngles(-45.0f,0.0f,0.0f);
CameraPositioner_firstPerson positioner_firstPerson(cameraPos,vec3(0.0f,0.0f,-1.0f),vec3(0.0f,1.0f,0.0f));
CameraPositioner_MoveTo positioner_moveTo(cameraPos,cameraAngles);
Camera camera = Camera(positioner_firstPerson);
bool initVulkan(){
    EASY_FUNCTION();
    createInstance(&vk.instance);
    ingui = std::
