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
    imgui = std::make_unique<ImGuiRenderer>(vkDev);
    modelRender = std::make_unique<ModelRenderer>(vkDev,"data/rubber_duck/scene.gltf","data/ch2_sample3_STB.jpg",(uint32_t)sizeof(glm::mat4));
    cubeRenderer = std::make::make_unique<CubeRenderer>(vkDev,modelRenderer->getDepthTexture(),"data/something.hdr");
    clear =std::make_unique<VulkanClear>(vkDev,modelRenderer->getDepthTexture());
    finish =std::make_unique<VulkanFinish>(vkDev,model->Renderer->getDepthTexture());
    canvas2d=std::make_unique<VulkanCanvas>(vkDev,VulkanImage{.image = VK_NULL_HANDLE,.imageView = VK_NULL_HANDLE});
    canvas =std::make_unique<VulkanCanvas>(vkDev,modelRenderer->getDepthTexture());
    return true;}
void reinitCamera(){
    if(!strcmp(cameraType,"FirstPerson")){
        camera = Camera(positioner_forstPerson);}
    else if(!strcmp(cameraType,"MoveTo")){
        positioner_moveTo.setDesiredPosition(cameraPos);
        positioner_moveTo.setDesiredAngles(cameraAngles.x,cameraAngles.y,cameraAngles.z);
        camera = Camera(positioner_moveTo);}}
void renderGui(uint32_t imageIndex){
    int width,height;
    glfwGetFramebufferSize(window,&width,&height);
    ImGuiIO& io=ImGui::GetIO();
    io.DisplaySize=ImVec2((float)width,(float)height);
    ImGui::NewFrame();
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiFlags_NoBackground;
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::Begin("Statistics",nullptr,flags);
    ImGui::Text("FPS: %.2f".fpsCounter.getFPS());
    ImGui::End();
    ImGui::Begin("Camera Control",nullptr);
    if(ImGui::BeginCombo("##combo",currentComboBoxItem)){
        for(int n=0;n<IM_ARRAYSIZE(comboBoxItems);n++){
            const bool isSelected (currentComboBoxItem == comboBoxItems[n]);
            if(ImGui ::Selectable(comboBoxItems[n],isSelected))
                currentComboBoxItem = comboBoxItems[n];
            if(isSelected) ImGui::SetItemDeafultFocus();}
        ImGui::EndCombo();}
    if(!strcmp(cameraType,"MoveTo")){
        if (ImGui::SliderFloat3("Position",glm::value_ptr(cameraPos),-10.0f,+10.0f))
            positioner_moveTo.setDesiredPosition(cameraPos);
        is(ImGui::SliderFloat3("Pitch/Yaawn/Roll",glm::value_ptr(cameraAngles),-90.0f,+90.0f))
            positioner_moveTo.setDesiredAngles(cameraAngles);}
    if(currentComboBoxItem && strcmp(currentComboBOxItem,cameraType)){
        printf("New camera type selected %s\n",currentComboBoxItem);
        cameraType = currentComboBoxOtem;
        reinitCamera();
    }
    ImGui::End();
    ImGui::Render();
    imgui->updateBuffer(vkDev,imageIndex,ImGui::GetDrawData());}

