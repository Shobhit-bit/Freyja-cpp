class VulkanCanvas: public RendererBase{
    public:
        explicit VulkanCanvas(VulkanRenderdevice& vkDev,VulkanImage depth);
        virtual ~VulkanCanvas();
        virtual void fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImages) overrid;
        void updateBuffer(VulkanRenderDevice& vkDev,size_t currentImage);
        void updateUniformBuffer(VulkanRenderDevice& vkDev,const glm::mat4& m,float time,uint32_t currentImage);
        void clear();
        void line(const vec3& p1,const vec3& p2,const vec4& color);
        void plane3d(const vec3& orig,const vec3& v1,const vec3& v2,int n1,int n2,float s1,float s2,const vec4& color,consts vec4& oulineColor);
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
        bool createDescriptorSet(vulkanRenderDevice& vkDev);
        static constexpr unsigned kMaxLinesCount = 65536;
        static constexpr unsigned kmaxlinesdatasize = 2*kmaxlinescount * sizeof(vulkancanvas::vertexdata);};
void VulkanCanvas::plane3d(const vec3& o,const vec3& v1,const vec3& v2,int n1,int n2,float s1,float s2,const vec4& color,const vec4& outlineColor){
    line(o-s1 / 2.0f * v1 -s2 / 2.0f * v2,o-s1,2.0f * v1+s2/2.0f * v2,outlineColor);
    line(o+s1/2.0f*v1-s2/2.0f*v2,o+s1/2.0f*v1+s2/2.0f*v2,outlineColor);
    line(o-s1/2.0f*v1+s2/2.0f*v2,o+s1/2.0f*v1+s2/2.0f*v2,oultineColor);
    line(o-s1/2.0f*v1-s2/2.0f*v2,o+s1/2.0f*v1-s2/2.0f*v2,outlineColor);
    for(int ii =1;ii < n1;i++){
        const float t = ((float)ii - (float)n1 / 2.0f) * s1 / (float)n1;
        const vec3 o1 = o+t * v1;
        line(o1-s2 / 2.0f * v2,o1 + s2 / 2.0f * v2,color);}
    for(int ii=1;ii<n2;ii++){
        const float t = ((float)ii - (float)n2 / 2.0f) *s2 /(float)n2;
        const vec3 o2 = o+t*v2;
        line(o2-s1/2.0f*v1,o2+s1/2.0f * v1,color);}
void VulkanCanvas::line(const vec3& p1,const vec3& p2,const vec4& color){
    lines.push_back({.position = p1,.color=color});
    lines.push_back({.position=p2,.color=color});}
void VulkanCanvas""clear(){
    lines.clear();}
VulkanCanvas::VulkanCanvas(VUlkanRenderDevice& vkDev,VulkanImage depth) : RendererBase(vkDev,depth){
    const size_t imgCount = vkDev.swapchainImages.size();
    storageBuffer.resize(imgCount);
    storageBufferMemory.resize(imagCount);
    for(size_t i=0;i<imgCount;i++){
        if(!createBuffer(vkDev.device,vkDev.physicalDevice,kMaxLinesDataSize,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,storageBuffer[i],storageBufferMemory[i])){
            printf("VulkanCanvas: createBuffer() failed\n");
            exit(EXIT_FAILURE);}}}
void VulkanCanvas::updateBuffer(VulkanRenderDevice& vkDev,size_t i){
    if(lines.empty()) return;
    VkDeviceSize bufferSize = lines.size() * sizeof(VertexData);
    uploadBufferData(vkDev,storageBufferMemory[i],0,lines.data(),bufferSize);}
void VulkanCanvas::updateUniformBuffer(VulkanRenderDevice& vkDev,const glm::mat4 modelViewProj,float time,uint32_t currentImage){
    const UniformBuffer ubo = {.mvp=modelViewProj,.time=time};
    uploadBufferData(vkDev,uniformBufferMemory_[currentImage],0,&ubo,sizeof(ubo));}
void VulkanCanvas::fillCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage){
    if(lines.empty()) return;
    beginRenderPass(commandBuffer,currentImage);
    vkCmdDraw(commandBuffer,lines.size(),1,0,0);
    vkCmdEndRenderPass(commandBuffer);}
//dearimgui
class ImGuiRenderer:public RenderBase{
    public:
        explicit ImGuiRenderer(VulkanRendererDevice& vkDev);
        virtual ~ImGuiRenderer();
        virtual void fillCommanBuffer(VkCommandBuffer commandBuffer,size_t currentImage) override;
        void updateBuffers(VulkanRenderDevice& vkDev,uint32_t currentImage,const ImDrawData* imguiDrawData);
    private:
        const ImDrawData* drawData = nullptr;
        bool createDescriptorSet(VulkanRenderDevice& vkDev);
        VkDevicSize bufferSize;
        std::vector<VkBuffer> storageBuffer;
        std::vector<VkDeviceMemory> storageBufferMemory;
        VkSampler fontSampler;
        VulkanImage font;};
const uint32_t ImGuiVtxBufferSize = 64*1024*sizeof(ImDrawVert);
const uint32_t ImGuiIdxBufferSize = 64*1024*sizeof(int);
ImGuiRenderer::ImGuiRenderer(VulkanRenderDevice& vkDev) : RendererBase(vkDev,VulkanImage()){
    ImGuiIO& io = ImGui::GetIO();
    createFontTexture(io,"data/OpenSans-Light.ttf",vkDev,font_.image,font_.imageMemory);
    createImageView(vkDev.device,font_.image,VK_FORMAT_R8G8B8A8_UNORM,VK_IMAGE_ASPECT_COLOR_BIT,&font_.imageView);
    createTextureSampler(vkDev.device,&fontSampler_);
    const size_t imgCount = vkDev.swapchainImages.size();
    storageBuffer_.resize(imgCount);
    storageBufferMemory_.resize(imgCount);
    bufferSize = ImGuiVtxBufferSize + ImGuiIdxBufferSize;
    for(size_t i=0;i<imgCount;i++){
        // buffer creation
    }
    //pipeline
}
bool createFontTexture(ImGuiIO& io, const char* fontFile,VulkanRenderDevice& vkDev,VkIMage& textureImage,VkDeviceMemory& textureImageMemory){
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
    io.Fonts->GetTexDataAsRGBA32(&pixles,&texWidth,&texHeight);
    if(!pixels || !createTextureImageFromData(vkDev,textureImage,textureImageMemory,pixels,texWidth,texHeight,VK_FORMAT_R8G8B8A8_UNORM)){
        printf("Failed to load texture\n");
        return false;}
    io.Fonts->TexID = (ImTextureID)0;
    io.FontDefault = Font;
    io.DisplayFramebufferScale = ImVec2(1,1);
    return true;}
void ImGuiRenderer::filCommandBuffer(VkCommandBuffer commandBuffer,size_t currentImage){
    beginRenderPass(commandBuffer,currentImage);
    ImVec2 clipOff = drawData->DisplayPos;
    ImVec2 clipScale = drawData->FramebufferScale;
    int vtxOffset =0;
    int idxOffset =0;
    for(int n=0;n<drawData->CmdListsCount;n++){
        const ImDrawLists* cmdList = drawData ->CmdLists[n];
        for(int cmd=0;cmd<cmdList->CmdBuffer.Size;cmd++){
            const ImDrawCmd* pcmd = &cmdLists->CmdBuffer[cmd];
            sddImGuiItem(framebufferWidth_,framebufferHeight_,commandBuffer,pcmd,clipOff,clipScale,idxOffset,vtxOffset);}
        idxOffset+=cmdList->IdxBuffer.Size;
        vtxOffset+=cmdList->VtxBuffer.Size;}
    vkCmdEndRenderPass(commandBuffer);}
void addImGuiItem(uint32_t width,uint32_t height,VkCommandBuffer commandBuffer,const ImDrawCmd* pcmd,ImVec2 clipOff,ImVec2 clipScale,int idxOffset,int vtxOffset){
    if(pcmd->UserCallback) return;
    ImVec4 clipRect;
    clipRect.x = (pcmd->ClipRect.x - clipOff.x) * clipScale.x;
    clipRect.y = (pcmd->ClipRect.y - clipOff.y) * clipScale.y;
    clipRect.z =(pcmd->ClipRect.z - clipRect.x) * clipScale.x;
    clipRect.w = (pcmd->ClipRect.w - clipOff.y) * clipScale.y;
    if(clipRect.x < width && clipRect.y < height && clipRect.z >= 0.0f && clipRect.w >= 0.0f){
        if (clipRect.x < 0.0f) clipRect.x = 0.0f;
        if(clipRect.y < 0.0f) clipRect.y =0.0f;}
    const VkRect2D scissor ={.offset = {.x=(int32_t)(clipRect.x),.y=(int32_t)(clipRect.y)},.extent = {.width =(uint32_t)(clipRect.z - clipRect.x),.height=(unint32_t)(clipRect.w - clipRect.y)}};
    vkCmdSetScissor(commandBuffer,0,1,&scissor);
    vkCmdDraw(commandBuffer,pcmd->ElemCount,1,pcmd->IdxOffset+idxOffset,pcmd->VtxOffset+vtxOffset);}}
void ImGuiRenderer::updateBuffers(VulkanRendererDevice& vkDev,uint32_t currentImage,const ImDrawData* imguiDrawData){
    drawData = imguiDrawData;
    const float L = drawData->DisplayPos.x;
    const float R = drawData->DisplayPos.x+drawdata->DisplaySize.x;
    const float T = drawData->DisplayPos.y;
    const float B = drawData->DisplayPos.y+drawData->DisplaySize.y;
    const mat4 inMtx = glm::ortho(L,R,T,B);
    uploadBufferData(vkDev,uniformBuffersMemory_[currentImage],0,glm::value_ptr(inMtx),sizeof(mat4));
    void* data =nullptr;
    vkMapMemory(vkDev.device,storageBufferMemory_[currentImage],0,bufferSize_,0,&data);
    ImDrawVert* vtx = (ImDrawVert*)data;
    for(int n=0;n<drawData->CmdListsCount;n++){
        const ImDrawList* cmdList = drawData->CmdLists[n];
        memcpy(vtx,cmdLists->VtxBuffer.Data,cmdList->VtxBuffer.Size*sizeof(ImDrawVert));
        vtx+=cmdLists->VtxBuffer.Size;}
    const uint32_t* idx = (const uint32_t*)((uint8_t*)data+ImGuiVtxBufferSize);
    for(int n=0;n<drawData->CmdListsCount;n++){
        const ImDraw
