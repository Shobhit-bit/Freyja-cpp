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

