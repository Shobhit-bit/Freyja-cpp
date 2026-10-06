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
        static sonstexpr unsigned kMaxLinesCount = 65536;
        static constexor unsigned kMaxLinesDataSize = 2*kMaxLinesCount * sizeof(VulkanCanvas::VertexData);};


