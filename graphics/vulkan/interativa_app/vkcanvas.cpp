#include <deque>
#include <limits>
class VulkanCanvas: public RendererBase{
    public:
        explicit VulkanCanvas(VulkanRenderDevice& vkDev,VulkanImage depth);
        virtual ~VulkanCanvas();
        virtual void fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage) override;
        void updateBuffer(VulkanRenderDevice& vkDev,size_t currentImage);
        void updateUniformBuffer(VulkanRenderDevice& vkDev,const glm::mat4& m,float time,uint32_t currentImage);
        void clear();
        void line(const vec3& p1,const vec3& p2,const vec4& color);
        void plane3d(const vec3& orig,const vec3& v1,const vec3& v2,int n1,int n2,float s1,float s2,const vec4& color,const vec4& outlineColor);
    private:
        struct VertexData{
            vec3 position;
            vec4 color;};
        struct UniformBuffer{
            glm::mat4 mvp;
            float time;};
        std::vector<VertexData> lines;
        std::vector<VkBuffer> storageBuffer;
        std::vector<VkDeviceMemory> storageBufferMemory;
        bool createDescriptorSet(VulkanRenderDevice& vkDev);
        static constexpr unsigned kMaxLinesCount = 65536;
        static constexpr unsigned kMaxLinesDataSize = 2*kMaxLinesCount * sizeof(VulkanCanvas::VertexData);};
void VulkanCanvas::plane3d(const vec3& o,const vec3& v1,const vec3& v2,int n1,int n2,float s1,float s2,const vec4& color,const vec4& outlineColor){
    line(o-s1 / 2.0f * v1 -s2 / 2.0f * v2,o-s1 / 2.0f * v1+s2/2.0f * v2,outlineColor);
    line(o+s1/2.0f*v1-s2/2.0f*v2,o+s1/2.0f*v1+s2/2.0f*v2,outlineColor);
    line(o-s1/2.0f*v1+s2/2.0f*v2,o+s1/2.0f*v1+s2/2.0f*v2,outlineColor);
    line(o-s1/2.0f*v1-s2/2.0f*v2,o+s1/2.0f*v1-s2/2.0f*v2,outlineColor);
    for(int ii =1;ii < n1;ii++){
        const float t = ((float)ii - (float)n1 / 2.0f) * s1 / (float)n1;
        const vec3 o1 = o+t * v1;
        line(o1-s2 / 2.0f * v2,o1 + s2 / 2.0f * v2,color);}
    for(int ii=1;ii<n2;ii++){
        const float t = ((float)ii - (float)n2 / 2.0f) *s2 /(float)n2;
        const vec3 o2 = o+t*v2;
        line(o2-s1/2.0f*v1,o2+s1/2.0f * v1,color);}}
void VulkanCanvas::line(const vec3& p1,const vec3& p2,const vec4& color){
    lines.push_back({.position = p1,.color=color});
    lines.push_back({.position=p2,.color=color});}
void VulkanCanvas::clear(){
    lines.clear();}
VulkanCanvas::VulkanCanvas(VulkanRenderDevice& vkDev,VulkanImage depth) : RendererBase(vkDev,depth){
    const size_t imgCount = vkDev.swapchainImages.size();
    storageBuffer.resize(imgCount);
    storageBufferMemory.resize(imgCount);
    for(size_t i=0;i<imgCount;i++){
        if(!createBuffer(vkDev.device,vkDev.physicalDevice,kMaxLinesDataSize,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,storageBuffer[i],storageBufferMemory[i])){
            printf("VulkanCanvas: createBuffer() failed\n");
            exit(EXIT_FAILURE);}}
    const bool useDepth = depth.image != VK_NULL_HANDLE;
    if(!createColorAndDepthRenderPass(vkDev,useDepth,&renderPass_,RenderPassCreateInfo()) ||
       !createUniformBuffers(vkDev,sizeof(UniformBuffer)) ||
       !createColorAndDepthFramebuffers(vkDev,renderPass_,depth.imageView,swapchainFramebuffers_) ||
       !createDescriptorPool(vkDev,1,1,0,&descriptorPool_) ||
       !createDescriptorSet(vkDev) ||
       !createPipelineLayout(vkDev.device,descriptorSetLayout_,&pipelineLayout_) ||
       !createGraphicsPipeline(vkDev,renderPass_,pipelineLayout_,{"data/shaders/chapter04/VKCanvas.vert","data/shaders/chapter04/VKCanvas.frag"},&graphicsPipeline_,VK_PRIMITIVE_TOPOLOGY_LINE_LIST,useDepth,true)){
        printf("VulkanCanvas: failed to create pipeline\n");
        exit(EXIT_FAILURE);}}
VulkanCanvas::~VulkanCanvas(){
    for(size_t i=0;i<storageBuffer.size();i++){
        vkDestroyBuffer(device_,storageBuffer[i],nullptr);
        vkFreeMemory(device_,storageBufferMemory[i],nullptr);}}
bool VulkanCanvas::createDescriptorSet(VulkanRenderDevice& vkDev){
    const std::array<VkDescriptorSetLayoutBinding,2> bindings = {
        descriptorSetLayoutBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT)};
    const VkDescriptorSetLayoutCreateInfo layoutInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,.pNext = nullptr,.flags=0,.bindingCount = static_cast<uint32_t>(bindings.size()),.pBindings=bindings.data()};
    VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device,&layoutInfo,nullptr,&descriptorSetLayout_));
    std::vector<VkDescriptorSetLayout> layouts(vkDev.swapchainImages.size(),descriptorSetLayout_);
    const VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,.pNext = nullptr,.descriptorPool=descriptorPool_,.descriptorSetCount = static_cast<uint32_t>(vkDev.swapchainImages.size()),.pSetLayouts = layouts.data()};
    descriptorSets_.resize(vkDev.swapchainImages.size());
    VK_CHECK(vkAllocateDescriptorSets(vkDev.device,&allocInfo,descriptorSets_.data()));
    for(size_t i=0;i<vkDev.swapchainImages.size();i++){
        VkDescriptorSet ds = descriptorSets_[i];
        const VkDescriptorBufferInfo bufferInfo = {uniformBuffers_[i],0,sizeof(UniformBuffer)};
        const VkDescriptorBufferInfo bufferInfo2 = {storageBuffer[i],0,kMaxLinesDataSize};
        const std::array<VkWriteDescriptorSet,2> descriptorWrites = {bufferWriteDescriptorSet(ds,&bufferInfo,0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
            bufferWriteDescriptorSet(ds,&bufferInfo2,1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)};
        vkUpdateDescriptorSets(vkDev.device,static_cast<uint32_t>(descriptorWrites.size()),descriptorWrites.data(),0,nullptr);}
    return true;}
void VulkanCanvas::updateBuffer(VulkanRenderDevice& vkDev,size_t i){
    if(lines.empty()) return;
    VkDeviceSize bufferSize = lines.size() * sizeof(VertexData);
    uploadBufferData(vkDev,storageBufferMemory[i],0,lines.data(),bufferSize);}
void VulkanCanvas::updateUniformBuffer(VulkanRenderDevice& vkDev,const glm::mat4& modelViewProj,float time,uint32_t currentImage){
    const UniformBuffer ubo = {.mvp=modelViewProj,.time=time};
    uploadBufferData(vkDev,uniformBuffersMemory_[currentImage],0,&ubo,sizeof(ubo));}
void VulkanCanvas::fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage){
    if(lines.empty()) return;
    beginRenderPass(commandBuffer,currentImage);
    vkCmdDraw(commandBuffer,static_cast<uint32_t>(lines.size()),1,0,0);
    vkCmdEndRenderPass(commandBuffer);}
//dearimgui
class ImGuiRenderer:public RendererBase{
    public:
        explicit ImGuiRenderer(VulkanRenderDevice& vkDev);
        virtual ~ImGuiRenderer();
        virtual void fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage) override;
        void updateBuffers(VulkanRenderDevice& vkDev,uint32_t currentImage,const ImDrawData* imguiDrawData);
    private:
        const ImDrawData* drawData = nullptr;
        bool createDescriptorSet(VulkanRenderDevice& vkDev);
        VkDeviceSize bufferSize_;
        std::vector<VkBuffer> storageBuffer_;
        std::vector<VkDeviceMemory> storageBufferMemory_;
        VkSampler fontSampler_;
        VulkanImage font_;};
const uint32_t ImGuiVtxBufferSize = 64*1024*sizeof(ImDrawVert);
const uint32_t ImGuiIdxBufferSize = 64*1024*sizeof(int);
bool createFontTexture(ImGuiIO& io, const char* fontFile,VulkanRenderDevice& vkDev,VkImage& textureImage,VkDeviceMemory& textureImageMemory){
    ImFontConfig cfg = ImFontConfig();
    cfg.FontDataOwnedByAtlas = false;
    cfg.RasterizerMultiply = 1.5f;
    cfg.SizePixels = 768.0f/32.0f;
    cfg.PixelSnapH = true;
    cfg.OversampleH = 4;
    cfg.OversampleV = 4;
    ImFont* Font = io.Fonts->AddFontFromFileTTF(fontFile,cfg.SizePixels,&cfg);
    unsigned char* pixels = nullptr;
    int texWidth,texHeight;
    io.Fonts->GetTexDataAsRGBA32(&pixels,&texWidth,&texHeight);
    if(!pixels || !createTextureImageFromData(vkDev,textureImage,textureImageMemory,pixels,texWidth,texHeight,VK_FORMAT_R8G8B8A8_UNORM)){
        printf("Failed to load texture\n");
        return false;}
    io.Fonts->TexID = (ImTextureID)0;
    io.FontDefault = Font;
    io.DisplayFramebufferScale = ImVec2(1,1);
    return true;}
ImGuiRenderer::ImGuiRenderer(VulkanRenderDevice& vkDev) : RendererBase(vkDev,VulkanImage()){
    ImGuiIO& io = ImGui::GetIO();
    createFontTexture(io,"data/OpenSans-Light.ttf",vkDev,font_.image,font_.imageMemory);
    createImageView(vkDev.device,font_.image,VK_FORMAT_R8G8B8A8_UNORM,VK_IMAGE_ASPECT_COLOR_BIT,&font_.imageView);
    createTextureSampler(vkDev.device,&fontSampler_);
    const size_t imgCount = vkDev.swapchainImages.size();
    storageBuffer_.resize(imgCount);
    storageBufferMemory_.resize(imgCount);
    bufferSize_ = ImGuiVtxBufferSize + ImGuiIdxBufferSize;
    for(size_t i=0;i<imgCount;i++){
        if(!createBuffer(vkDev.device,vkDev.physicalDevice,bufferSize_,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,storageBuffer_[i],storageBufferMemory_[i])){
            printf("ImGuiRenderer: createBuffer() failed\n");
            exit(EXIT_FAILURE);}}
    if(!createColorAndDepthRenderPass(vkDev,false,&renderPass_,RenderPassCreateInfo()) ||
       !createColorAndDepthFramebuffers(vkDev,renderPass_,VK_NULL_HANDLE,swapchainFramebuffers_) ||
       !createUniformBuffers(vkDev,sizeof(mat4)) ||
       !createDescriptorPool(vkDev,1,2,1,&descriptorPool_) ||
       !createDescriptorSet(vkDev) ||
       !createPipelineLayout(vkDev.device,descriptorSetLayout_,&pipelineLayout_) ||
       !createGraphicsPipeline(vkDev,renderPass_,pipelineLayout_,{"data/shaders/chapter04/imgui.vert","data/shaders/chapter04/imgui.frag"},&graphicsPipeline_,VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,true,true,true)){
        printf("ImGuiRenderer: pipeline creation failed\n");
        exit(EXIT_FAILURE);}}
ImGuiRenderer::~ImGuiRenderer(){
    for(size_t i=0;i<storageBuffer_.size();i++){
        vkDestroyBuffer(device_,storageBuffer_[i],nullptr);
        vkFreeMemory(device_,storageBufferMemory_[i],nullptr);}
    vkDestroySampler(device_,fontSampler_,nullptr);
    destroyVulkanImage(device_,font_);}
bool ImGuiRenderer::createDescriptorSet(VulkanRenderDevice& vkDev){
    const std::array<VkDescriptorSetLayoutBinding,4> bindings = {
        descriptorSetLayoutBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(2,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(3,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT)};
    const VkDescriptorSetLayoutCreateInfo layoutInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,.pNext = nullptr,.flags=0,.bindingCount = static_cast<uint32_t>(bindings.size()),.pBindings=bindings.data()};
    VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device,&layoutInfo,nullptr,&descriptorSetLayout_));
    std::vector<VkDescriptorSetLayout> layouts(vkDev.swapchainImages.size(),descriptorSetLayout_);
    const VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,.pNext = nullptr,.descriptorPool=descriptorPool_,.descriptorSetCount = static_cast<uint32_t>(vkDev.swapchainImages.size()),.pSetLayouts = layouts.data()};
    descriptorSets_.resize(vkDev.swapchainImages.size());
    VK_CHECK(vkAllocateDescriptorSets(vkDev.device,&allocInfo,descriptorSets_.data()));
    for(size_t i=0;i<vkDev.swapchainImages.size();i++){
        VkDescriptorSet ds = descriptorSets_[i];
        const VkDescriptorBufferInfo bufferInfo = {uniformBuffers_[i],0,sizeof(mat4)};
        const VkDescriptorBufferInfo bufferInfo2 = {storageBuffer_[i],0,ImGuiVtxBufferSize};
        const VkDescriptorBufferInfo bufferInfo3 = {storageBuffer_[i],ImGuiVtxBufferSize,ImGuiIdxBufferSize};
        const VkDescriptorImageInfo imageInfo = {fontSampler_,font_.imageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        const std::array<VkWriteDescriptorSet,4> descriptorWrites = {
            bufferWriteDescriptorSet(ds,&bufferInfo,0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
            bufferWriteDescriptorSet(ds,&bufferInfo2,1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
            bufferWriteDescriptorSet(ds,&bufferInfo3,2,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
            imageWriteDescriptorSet(ds,&imageInfo,3)};
        vkUpdateDescriptorSets(vkDev.device,static_cast<uint32_t>(descriptorWrites.size()),descriptorWrites.data(),0,nullptr);}
    return true;}
void addImGuiItem(uint32_t width,uint32_t height,VkCommandBuffer commandBuffer,const ImDrawCmd* pcmd,ImVec2 clipOff,ImVec2 clipScale,int idxOffset,int vtxOffset){
    if(pcmd->UserCallback) return;
    ImVec4 clipRect;
    clipRect.x = (pcmd->ClipRect.x - clipOff.x) * clipScale.x;
    clipRect.y = (pcmd->ClipRect.y - clipOff.y) * clipScale.y;
    clipRect.z =(pcmd->ClipRect.z - clipOff.x) * clipScale.x;
    clipRect.w = (pcmd->ClipRect.w - clipOff.y) * clipScale.y;
    if(clipRect.x < width && clipRect.y < height && clipRect.z >= 0.0f && clipRect.w >= 0.0f){
        if (clipRect.x < 0.0f) clipRect.x = 0.0f;
        if(clipRect.y < 0.0f) clipRect.y =0.0f;
        const VkRect2D scissor ={.offset = {.x=(int32_t)(clipRect.x),.y=(int32_t)(clipRect.y)},.extent = {.width =(uint32_t)(clipRect.z - clipRect.x),.height=(uint32_t)(clipRect.w - clipRect.y)}};
        vkCmdSetScissor(commandBuffer,0,1,&scissor);
        vkCmdDraw(commandBuffer,pcmd->ElemCount,1,pcmd->IdxOffset+idxOffset,pcmd->VtxOffset+vtxOffset);}}
void ImGuiRenderer::fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage){
    beginRenderPass(commandBuffer,currentImage);
    ImVec2 clipOff = drawData->DisplayPos;
    ImVec2 clipScale = drawData->FramebufferScale;
    int vtxOffset =0;
    int idxOffset =0;
    for(int n=0;n<drawData->CmdListsCount;n++){
        const ImDrawList* cmdList = drawData ->CmdLists[n];
        for(int cmd=0;cmd<cmdList->CmdBuffer.Size;cmd++){
            const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmd];
            addImGuiItem(framebufferWidth_,framebufferHeight_,commandBuffer,pcmd,clipOff,clipScale,idxOffset,vtxOffset);}
        idxOffset+=cmdList->IdxBuffer.Size;
        vtxOffset+=cmdList->VtxBuffer.Size;}
    vkCmdEndRenderPass(commandBuffer);}
void ImGuiRenderer::updateBuffers(VulkanRenderDevice& vkDev,uint32_t currentImage,const ImDrawData* imguiDrawData){
    drawData = imguiDrawData;
    const float L = drawData->DisplayPos.x;
    const float R = drawData->DisplayPos.x+drawData->DisplaySize.x;
    const float T = drawData->DisplayPos.y;
    const float B = drawData->DisplayPos.y+drawData->DisplaySize.y;
    const mat4 inMtx = glm::ortho(L,R,T,B);
    uploadBufferData(vkDev,uniformBuffersMemory_[currentImage],0,glm::value_ptr(inMtx),sizeof(mat4));
    void* data =nullptr;
    vkMapMemory(vkDev.device,storageBufferMemory_[currentImage],0,bufferSize_,0,&data);
    ImDrawVert* vtx = (ImDrawVert*)data;
    for(int n=0;n<drawData->CmdListsCount;n++){
        const ImDrawList* cmdList = drawData->CmdLists[n];
        memcpy(vtx,cmdList->VtxBuffer.Data,cmdList->VtxBuffer.Size*sizeof(ImDrawVert));
        vtx+=cmdList->VtxBuffer.Size;}
    uint32_t* idx = (uint32_t*)((uint8_t*)data+ImGuiVtxBufferSize);
    for(int n=0;n<drawData->CmdListsCount;n++){
        const ImDrawList* cmdList = drawData->CmdLists[n];
        const uint16_t* src = (const uint16_t*)cmdList->IdxBuffer.Data;
        for(int j=0;j<cmdList->IdxBuffer.Size;j++)
            *idx++ = (uint32_t)*src++;}
    vkUnmapMemory(vkDev.device,storageBufferMemory_[currentImage]);}
class LinearGraph{
    std::deque<float> graph_;
    const size_t maxPoints_;
public:
    explicit LinearGraph(size_t maxGraphPoints = 256) : maxPoints_(maxGraphPoints){}
    void addPoint(float value){
        graph_.push_back(value);
        if(graph_.size() > maxPoints_)
            graph_.pop_front();}
    void renderGraph(VulkanCanvas& c,const glm::vec4& color = vec4(1.0)) const{
        float minfps = std::numeric_limits<float>::max();
        float maxfps = std::numeric_limits<float>::lowest();
        for(float f: graph_){
            if(f<minfps) minfps =f;
            if(f>maxfps) maxfps = f;}
        const float range = std::max(maxfps - minfps,1e-6f);
        float x=0.0;
        vec3 p1 = vec3(0,0,0);
        const float scalingFactor = 0.15f;
        for(float f:graph_){
            const float val = (f-minfps) / range;
            const vec3 p2=vec3(x,val*scalingFactor,0);
            x+=1.0f/maxPoints_;
            c.line(p1,p2,color);
            p1=p2;}}};
// (the stray main-loop snippet that was at the bottom of this file now lives in main() in vkdemof.cpp)
