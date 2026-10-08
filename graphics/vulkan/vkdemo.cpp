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
    if(!createTexturedVertexBuffer(vkDev,"data/rubber_duck/scene.gltf",&vkState.storageBuffer,&vkState.storageBufferMemory,&vertexBufferSize,&indexBufferSize) || !createUniformBuffers()){
        printf("Cannot create data buffers\n");
        exit(EXIT_FAILURE);}
    const std::vector<VkPipelineShaderStageCreateInfo>
        shaderStages = { shaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT,vkState.vertShader,"main"),
            shaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT,vkState.fragShader,"main"),
            shaderStafeInfo(VK_SHADER_STAGE_GEOMETRY_BIT,vkState.geomShader,"main")};
    createTextureImage(vkDev,"data/rubber_duck/textures/Duck_baseColor.png",vkState.texture.image,vkState.texture.imageMemory);
    createImageView(vkDev.device,vkState.texture.image,VK_FORMAT_R8B8G8A8_UNORM,VK_IMAGE_ASPECT_COLOR_BIT,&vkState.texture.imageView);
    createTextureSampler(vkDev.device,&vkState.textureSampler);
    createDepthResources(vkDev,kScreenWidth,kScreenHeight,vkState.depthTexture);
    const bool isIntialized = createDescriptorPool(vkDev.device,static_cast<uint32_t>(vkDev.swapchainImages.size()),1,2,1,&vkState.descriptorPool) && createDescriptorSet() && createColorAndDepthRenderPass(vkDev,true,&vkState.renderPass,RenderPassCreateInfo{.clearColor_ = true,.clearDepth_=true,.flags_ =eRenderPassBit_First|eRenderPassBit_Last}) && createPipelineLayout(vkDev.device,vkStatedescriptorSetLayout,&vkState.pipelineLayout) && createGraphicsPipeline(vkDev.device,kScreenWidth,kScrrenHeight,vkState.renderPass,vkState.pipelineLayout,shaderStages,&vkState.graphicsPipeline);
    if(!isInitialized){
        printf("failed to pipeline\n");
        exit(EXIT_FAILURE);}
    createColorAndDepthFramebuffers(vkDev,vkState.renderPass,vkState.depthTexture.imageView,kScreenWidth,kScreenHeight,vkState.swapchainFramebuffers);
    return VK_SUCCESS;}
bool drawOverlay(){
    uint32_t imageIndex =0;
    VK_CHECK(vkAcquireNextImageKHR(vkDev.device,vkDev.swapchain,0,vkDev.semaphore,VK_NULL_HANDLE,&imageIndex);
            VK_CHECK(vkResetCommandPool(vkDev.device,vkDev.commandPool,0));
    int width,height;
    glfwGetFramebufferSize(window,&width,&height);
    const float ratio = width/(float)height;
    const mat4 ml = glm::rotate(glm::translate(mat4(1.0f),vec3(0.f,0.5f,-1.5f)) * glm::rotate(mat4(1.f),glm::pi<float>(),vec3(1,0,0)),(float)glfwGetTime(),vec3(0.0f,1.0f,0.0f));
    const mat4 p =glm::persetective(45.0f,ratio,0.1f,1000.0f);
    const UniformBuffer ubo{.mvp = p*ml};
    updateUniformBuffer(imageIndex,ubo);
    fillCommandBuffer();
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    const VkSubmitInfo si ={.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO,.pNext = nullptr,.waitSemaphoreCount = 1,.pWaitSemaphore=&vkDev.semaphore,.pWaitDstStageMask=waitStages,.commandBufferCount=1,.pCommandBuffers=&vkDev.commandBuffer[imageIndex],.signalSemaphoreCount=1,.pSignalSemaphore = &vkDev.renderSemaphore};
    VK_CHECK(vkQueueSubmit(vkDev.graphicsQueue,1,&si,nullptr));
    const VkPresentInfoKHR pi= {.sType=VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,.pNext=nullptr,.pWaitSemaphore=&vkDev.renderSemaphore,.swapchainCount=1,.pSwapchains=&vkDev.swapchain,.pImageIndicies=&imageIndex};
    VK_CHECK(vkQueuePresentKHR(vkDev.graphicsQueue,&pi));
    VK_CHECK(vkDeviceWaitIdle(vkDev.device));
    return true;}
    glm::vec3 cameraPos(0.0f,0.0f,0.0f);
    glm::vec3 cameraAngles(-45.0f,0.0f,0.0f);
    CameraPositioner_FirstPerson positioner_firstPerson(cameraPos,vec3(0.0f,0.0f,-1.0f),vec3(0.0f,1.0f,0.0f));
    CameraPositioner_MoveTopositioner_moveTo(cameraPos,cameraAngles);
    Camera camera = Camera(positioner_firstPerson);
    positioner_firstPerson.update(deltaSeconds,mouseState.pos,mouseState.pressedLeft);
    const char* cameraType = "FirstPerson";
    const char* comboBoxItems[]={"FirstPerson","MoveTo"};
    const char* currentComboBoxItem = cameraType;
    ImGui::Begin("Camera Control",nullptr);{
        if(ImGui::BeginCombo("##combo",currentComboBoxItem)){
            for(int n=0;n<IM_ARRAYSIZE(comboBoxItems);n++){
                const bool isSelected = (currentComboBoxItem == comboBoxItems[n]);
                if(ImGui::Selectable(comboBoxItems[n],isSelected))
                    currentComboBoxItem = comboBoxItems[n];
                if(isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();}
        if(!strcmp(cameraType,"MoveTo")){
            if(ImGui::SliderFloat3("Position",glm::value_ptr(cameraPos,-10.0f,+10.0f)))
                positioner_moveTo.setDesiredPostition(cameraPos);
            if (ImGui::SliderFloat3("pitch/Pan/Roll",glm::valur_ptr(cameraAngles),-90.0f,+90.0f))
                positioner_moveTo.setDesiredAngles(cameraAngles);}
    if(currentComboBoxItem && strcmp(currentComboBoxItem,cameraType)){
        printf("New camera type selected %s\n",currentComboBoxItem);
        cameraType = currentComboBoxItem;
        reinitCamera();}}
class CameraPositioner_MoveTo final:
    public CameraPositionerInterface{
            public :
            float damplingLinear_ = 10.0f;
            glm::vec3 dampingEularAngles_ = glm::vec3(5.0f,5.0f,5.0f);
            private:
            glm::vec3 positionCurrent_ = glm::vec3(0.0f);
            glm::vec3 positionDesired_ = glm::vec3(0.0f);
            glm::vec3 anglesCurrent_ = glm::vec3(0.0f);
            glm::vec3 anglesDesired_ = glm::vec3(0.0f);
            glm::mat4 currentTransform_ = glm::mat4(1.0f);
            public:
            CameraPositioner_MoveTo(const glm::vec3& pos,const glm::vec3& angles):positionCurrent_(pos),positionDesired_(pos),anglesCurrent_(angles),anglesDesired_(angles){}
    void update(float deltaSeconds,const glm::vec2& mousePos ,bool mousePressed){
        positionCurrent_+=dampingLinear_ * deltaSeconds * (positionDesired_ - positionCurrent_);
        anglesCurrent_ = clipAngles(anglesCurrent_);
        anglesDesired_ = clipAngles(anglesDesired_);
        anglesCurrent_ -= deltaSeconds * angledelta(anglesCurrent_,anglesDesired_) * dampingEulerAngles_;
        anglesCurrent_ = clipAngles(anglesCurrent_);
        const glm::vec3 ang = glm::radians(anglesCurrent_);
        currentTransform_ = glm::translate(glm::yawPitchRoll(ang.y,ang.x,ang.z),-positionCurrent_);}
            private:
            static inline float clipAngle(float d){
                if(d< -180.0f) return d+360.0f;
                if(d>+180.0f) return d-360.f;
                return d;}
            static inline glm::vec3 clipAngles(const glm::vec3& angles){
                return glm::vec3(std::fmod(angles.x,360.0f),
                                std::fmod(angles.y,360.0f),
                                std::fmod(angles.x,360.0f));
            }
        static inline glm::vec3 angleDelta (const glm::vec3& anglesCurrent,const glm::vec3& anglesDesired){
            const glm::vec3 d=clipAngles(anglesCurrent) -clipAngles(anglesDesired);
            return glm::vec3(clipAngle(d.x),clipAngle(d.y),clipAngle(d.z));}};


