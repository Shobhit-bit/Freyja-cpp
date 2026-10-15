#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <easy/profiler.h>
// + your framework headers (VulkanRenderDevice, RendererBase, VulkanClear, VulkanFinish, ...)
// + the other four files from this project: vkcamera.cpp, vktex.cpp, vkrenderer.cpp, vkcanvas.cpp
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

const uint32_t kScreenWidth = 1280;
const uint32_t kScreenHeight = 720;
GLFWwindow* window = nullptr;
const char* cameraType = "FirstPerson";
const char* comboBoxItems[] = {"FirstPerson","MoveTo"};
const char* currentComboBoxItem = cameraType;
struct MouseState{
    glm::vec2 pos = glm::vec2(0.0f);
    bool pressedLeft = false;} mouseState;

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
CameraPositioner_FirstPerson positioner_firstPerson(cameraPos,vec3(0.0f,0.0f,-1.0f),vec3(0.0f,1.0f,0.0f));
CameraPositioner_MoveTo positioner_moveTo(cameraPos,cameraAngles);
Camera camera = Camera(positioner_firstPerson);

bool isDeviceSuitable(VkPhysicalDevice d){
    VkPhysicalDeviceFeatures f;
    vkGetPhysicalDeviceFeatures(d,&f);
    return f.geometryShader == VK_TRUE;}
bool initVulkan(){
    EASY_FUNCTION();
    createInstance(&vk.instance);
    if(!setupDebugCallbacks(vk.instance,&vk.messenger,&vk.reportCallback) ||
       glfwCreateWindowSurface(vk.instance,window,nullptr,&vk.surface) ||
       !initVulkanRenderDevice(vk,vkDev,kScreenWidth,kScreenHeight,isDeviceSuitable,{.geometryShader = VK_TRUE}))
        exit(EXIT_FAILURE);
    imgui = std::make_unique<ImGuiRenderer>(vkDev);
    modelRenderer = std::make_unique<ModelRenderer>(vkDev,"data/rubber_duck/scene.gltf","data/ch2_sample3_STB.jpg",(uint32_t)sizeof(glm::mat4));
    cubeRenderer = std::make_unique<CubeRenderer>(vkDev,modelRenderer->getDepthTexture(),"data/something.hdr");
    clear =std::make_unique<VulkanClear>(vkDev,modelRenderer->getDepthTexture());
    finish =std::make_unique<VulkanFinish>(vkDev,modelRenderer->getDepthTexture());
    canvas2d=std::make_unique<VulkanCanvas>(vkDev,VulkanImage{.image = VK_NULL_HANDLE,.imageView = VK_NULL_HANDLE});
    canvas =std::make_unique<VulkanCanvas>(vkDev,modelRenderer->getDepthTexture());
    return true;}
void reinitCamera(){
    if(!strcmp(cameraType,"FirstPerson")){
        camera = Camera(positioner_firstPerson);}
    else if(!strcmp(cameraType,"MoveTo")){
        positioner_moveTo.setDesiredPosition(cameraPos);
        positioner_moveTo.setDesiredAngles(cameraAngles.x,cameraAngles.y,cameraAngles.z);
        camera = Camera(positioner_moveTo);}}
void renderGUI(uint32_t imageIndex){
    int width,height;
    glfwGetFramebufferSize(window,&width,&height);
    ImGuiIO& io=ImGui::GetIO();
    io.DisplaySize=ImVec2((float)width,(float)height);
    ImGui::NewFrame();
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground;
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::Begin("Statistics",nullptr,flags);
    ImGui::Text("FPS: %.2f",fpsCounter.getFPS());
    ImGui::End();
    ImGui::Begin("Camera Control",nullptr);
    if(ImGui::BeginCombo("##combo",currentComboBoxItem)){
        for(int n=0;n<IM_ARRAYSIZE(comboBoxItems);n++){
            const bool isSelected = (currentComboBoxItem == comboBoxItems[n]);
            if(ImGui::Selectable(comboBoxItems[n],isSelected))
                currentComboBoxItem = comboBoxItems[n];
            if(isSelected) ImGui::SetItemDefaultFocus();}
        ImGui::EndCombo();}
    if(!strcmp(cameraType,"MoveTo")){
        if (ImGui::SliderFloat3("Position",glm::value_ptr(cameraPos),-10.0f,+10.0f))
            positioner_moveTo.setDesiredPosition(cameraPos);
        if(ImGui::SliderFloat3("Pitch/Yaw/Roll",glm::value_ptr(cameraAngles),-90.0f,+90.0f))
            positioner_moveTo.setDesiredAngles(cameraAngles);}
    if(currentComboBoxItem && strcmp(currentComboBoxItem,cameraType)){
        printf("New camera type selected %s\n",currentComboBoxItem);
        cameraType = currentComboBoxItem;
        reinitCamera();
    }
    ImGui::End();
    ImGui::Render();
    imgui->updateBuffers(vkDev,imageIndex,ImGui::GetDrawData());}
void update3D(uint32_t imageIndex){
    int width,height;
    glfwGetFramebufferSize(window,&width,&height);
    const float ratio = width/(float)height;
    const mat4 t = glm::translate(mat4(1.0f),vec3(0.0f,0.5f,-1.5f));
    const float angle = (float)glfwGetTime();
    const mat4 ml = glm::rotate(t* glm::rotate(mat4(1.f),glm::pi<float>(),vec3(1,0,0)),angle,vec3(0.0f,1.0f,0.0f));
    const mat4 p =glm::perspective(45.0f,ratio,0.1f,1000.0f);
    const mat4 view = camera.getViewMatrix();
    const mat4 mtx = p*view*ml;
    EASY_BLOCK("UpdateUniformBuffers");
    modelRenderer->updateUniformBuffer(vkDev,imageIndex,glm::value_ptr(mtx),sizeof(mat4));
    canvas->updateUniformBuffer(vkDev,p*view,0.0f,imageIndex);
    canvas2d->updateUniformBuffer(vkDev,glm::ortho(0.0f,1.0f,1.0f,0.0f),0.0f,imageIndex);
    cubeRenderer->updateUniformBuffer(vkDev,imageIndex,p*view*ml);
    EASY_END_BLOCK;}
void update2D(uint32_t imageIndex){
    canvas2d->clear();
    sineGraph.renderGraph(*canvas2d.get(),vec4(0.f,1.f,0.f,1.f));
    fpsGraph.renderGraph(*canvas2d.get());
    canvas2d->updateBuffer(vkDev,imageIndex);}
void composeFrame(uint32_t imageIndex,const std::vector<RendererBase*>& renderers){
    update3D(imageIndex);
    renderGUI(imageIndex);
    update2D(imageIndex);
    EASY_BLOCK("FillCommandBuffers");
    VkCommandBuffer commandBuffer = vkDev.commandBuffers[imageIndex];
    const VkCommandBufferBeginInfo bi = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,.pNext = nullptr,.flags=VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,.pInheritanceInfo = nullptr};
    VK_CHECK(vkBeginCommandBuffer(commandBuffer,&bi));
    for(auto& r:renderers) r->fillCommandBuffer(commandBuffer,imageIndex);
    VK_CHECK(vkEndCommandBuffer(commandBuffer));
    EASY_END_BLOCK;}
bool drawFrame(const std::vector<RendererBase*>&renderers){
    EASY_FUNCTION();
    uint32_t imageIndex =0;
    VkResult result = vkAcquireNextImageKHR(vkDev.device,vkDev.swapchain,0,vkDev.semaphore,VK_NULL_HANDLE,&imageIndex);
    VK_CHECK(vkResetCommandPool(vkDev.device,vkDev.commandPool,0));
    if(result != VK_SUCCESS) return false;
    composeFrame(imageIndex,renderers);
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    const VkSubmitInfo si ={.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,.pNext = nullptr,.waitSemaphoreCount =1,.pWaitSemaphores = &vkDev.semaphore,.pWaitDstStageMask = waitStages,.commandBufferCount = 1,.pCommandBuffers = &vkDev.commandBuffers[imageIndex],.signalSemaphoreCount=1,.pSignalSemaphores = &vkDev.renderSemaphore};
    EASY_BLOCK("vkQueueSubmit",profiler::colors::Magenta);
    VK_CHECK(vkQueueSubmit(vkDev.graphicsQueue,1,&si,nullptr));
    EASY_END_BLOCK;
    const VkPresentInfoKHR pi = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,.pNext = nullptr , .waitSemaphoreCount =1,.pWaitSemaphores = &vkDev.renderSemaphore,.swapchainCount = 1,.pSwapchains = &vkDev.swapchain,.pImageIndices = &imageIndex};
    EASY_BLOCK("vkQueuePresentKHR",profiler::colors::Magenta);
    VK_CHECK(vkQueuePresentKHR(vkDev.graphicsQueue,&pi));
    EASY_END_BLOCK;
    EASY_BLOCK("vkDeviceWaitIdle",profiler::colors::Red);
    VK_CHECK(vkDeviceWaitIdle(vkDev.device));
    EASY_END_BLOCK;
    return true;}

int main(){
    glslang_initialize_process();   // remove if your framework doesn't compile shaders at runtime
    if(!glfwInit()) return EXIT_FAILURE;
    if(!glfwVulkanSupported()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
    window = glfwCreateWindow(kScreenWidth,kScreenHeight,"VulkanApp",nullptr,nullptr);
    if(!window){ glfwTerminate(); return EXIT_FAILURE;}
    ImGui::CreateContext();
    glfwSetCursorPosCallback(window,[](GLFWwindow* window,double x,double y){
        ImGui::GetIO().MousePos = ImVec2((float)x,(float)y);
        int width,height;
        glfwGetFramebufferSize(window,&width,&height);
        mouseState.pos.x = static_cast<float>(x/width);
        mouseState.pos.y = static_cast<float>(y/height);});
    glfwSetMouseButtonCallback(window,[](GLFWwindow* window,int button,int action,int mods){
        auto& io = ImGui::GetIO();
        const int idx = button == GLFW_MOUSE_BUTTON_LEFT ? 0 : button == GLFW_MOUSE_BUTTON_RIGHT ? 2 : 1;
        io.MouseDown[idx] = action == GLFW_PRESS;
        if(!io.WantCaptureMouse)
            mouseState.pressedLeft = action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT;});
    glfwSetKeyCallback(window,[](GLFWwindow* window,int key,int scancode,int action,int mods){
        const bool press = action != GLFW_RELEASE;
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window,GLFW_TRUE);
        if(key == GLFW_KEY_W) positioner_firstPerson.movement_.forward_ = press;
        if(key== GLFW_KEY_S) positioner_firstPerson.movement_.backward_= press;
        if(key==GLFW_KEY_A) positioner_firstPerson.movement_.left_ = press;
        if(key==GLFW_KEY_D) positioner_firstPerson.movement_.right_ = press;
        if(key==GLFW_KEY_1) positioner_firstPerson.movement_.up_ = press;
        if(key==GLFW_KEY_2) positioner_firstPerson.movement_.down_ = press;
        if (mods & GLFW_MOD_SHIFT) positioner_firstPerson.movement_.fastSpeed_=press;
        if(key==GLFW_KEY_SPACE) positioner_firstPerson.setUpVector(vec3(0.0f,1.0f,0.0f));});

    initVulkan();
    // must be built after initVulkan(), when the unique_ptrs are populated
    const std::vector<RendererBase*> renderers = {clear.get(),cubeRenderer.get(),modelRenderer.get(),canvas.get(),canvas2d.get(),imgui.get(),finish.get()};

    double timeStamp = glfwGetTime();
    float deltaSeconds = 0.0f;
    while(!glfwWindowShouldClose(window)){
        const double newTimeStamp = glfwGetTime();
        deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
        timeStamp = newTimeStamp;
        positioner_firstPerson.update(deltaSeconds,mouseState.pos,mouseState.pressedLeft);
        positioner_moveTo.update(deltaSeconds,mouseState.pos,mouseState.pressedLeft);
        const bool frameRendered = drawFrame(renderers);
        if(fpsCounter.tick(deltaSeconds,frameRendered))
            fpsGraph.addPoint(fpsCounter.getFPS());
        sineGraph.addPoint((float)sin(glfwGetTime() * 10));
        glfwPollEvents();}

    imgui = nullptr;
    modelRenderer = nullptr;
    cubeRenderer = nullptr;
    canvas = nullptr;
    canvas2d = nullptr;
    clear = nullptr;
    finish = nullptr;
    destroyVulkanRenderDevice(vkDev);
    destroyVulkanInstance(vk);
    glfwDestroyWindow(window);
    glfwTerminate();
    glslang_finalize_process();
    return 0;}
