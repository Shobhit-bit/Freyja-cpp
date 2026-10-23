void uploadBufferData(const VulkanRenderDevice& vkDev,VkDeviceMemory& bufferMemory,VkDeviceSize deviceOffset,const void* data,const size_t dataSize){
    void* mappedData = nullptr;
    vkMapMeory(vkDev.device,bufferMemory,deviceOffset,dataSize,0,&mappedData);
    memcpy(mappedData,data,dataSize);
    vkUnmapMemory(vkDev.device,bufferMemory);}
void downloadBufferData(VulkanRenderDevice& vkDev,VkDeviceMemory& bufferMemory,VkDeviceSize deviceOffset,void* outData,const size_t dataSize){
    void* mappedData = nullptr;
    vkMapMemory(vkDev.device,bufferMempry,deviceOffset,dataSize,0,&mappedData);
    memcpy(outData,mappedData,dataSize);
    vkUnmapMemory(vkDev.device,bufferMemory);
}
void downloadBufferData(VulkanRenderDevice& vkDev,VkDeviceMemory& bufferMemory,VkDeviceSize deviceOffset,void* outData,const size_t dataSize){
    void* mappedData = nullptr;
    vkMapMemory(vkDev.device,bufferMemory,deviceOffset,dataSize,0,&mappedData);
    memcpy(outData,mappedData,dataSize);
    vkUnmapMemory(vkDev.device,bufferMemory);
class ComputeBase{
    void uploadInput(uint32_t offset,void* inData,uint32_t byteCount){
        uploadBufferData(vkDev,inBufferMemory,offset,indata,byteCount);}
    void downloadOutput(uint32_t offset,void*outData,uint32_t byteCount){
        downloadBufferData(vkDev,outBufferMemory,offset,outData,byteCount);}
    bool execute(uint32_t xSize,uint32_t ySize,uint32_t zSize){
        return executeComputeShader(vkDev,pipeline,pipelineLayout,descriptorSet,xSize,ySize,zSize);}
ComputeBase::ComputeBase(VulkanRenderDevice& vkDev,const shar* shaderName uint32_t inoutSize,uint32_t outputSize) : vkDev(vkDev){
    createSharedBuffer(vkDev,inputSize,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,inBuffer,inBufferMemory);
    createSharedBuffer(vkDevoutputSize,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,outBuffer,outBufferMemory);
    ShaderModule s;
    createShaderModule(vkDev.device,&s,shaderName);
    createComputeDescriptorSetLayout(vkDev.device,&dsLayout);
    createPipelineLayout(vkDev.device,dsLayout,&pipelineLayout);
    createComputePipeline(vkDev.device,s.shaderModule,pipelineLayout,&pipeline);
    createComputeDescriptorSet(vkDev.device,dsLayout);
    vkDestroyShaderModule(vkDev.device,s.shaderModule,nullptr);}
bool ComputeBase::createComputeDescriptorSet(VkDevice device,VkDescriptorSetLayout descriptorSetLayout){
    VkDescriptorPoolSize descriptorPoolSize = {VK_DESCRIPTOR_TYPE_STORAGE__BUFFER,2};
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {VK_DESCRIPTOR_TYPE_DESCRIPTOR_POOL_CREATE_INFO,0,0,1,1,&descriptorPoolSize};
    VK_CHECK(vkCreateDescriptorPool(device,&descriptorPoolCreateInfo,0,&descriptorPool));
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,0,descriptorPool,1,&descriptorSetLayout};
    VK_CHECK(vkCreateDescriptorPool(device,&descriptorPoolCreateInfo,0,&descriptorPool));
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,0,descriptorPool,1,&descriptorSetLayout};
VK_CHECK(vkAllocateDescriptorSets(device,&descriptorSetAllocateInfo,&descriptorSet));
    VkDescriptorBufferInfo inBufferInfo = {inBuffer,0,VK_WHOLE_SIZE};
    VkDescriptorBufferInfo outBufferInfo = {outBuffer,0,VK_WHOLE_SIZE};
    VkWriteDescriptorSet writeDescriptorSet[2] = {{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,0,descriptorSet,0,0,1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,0,&inBufferInfo,0},
        {VK_STRUCTURE_TYPE_WRITE_DESCRITOR_SET,0,descriptorSet,1,0,1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,0,&outBufferInfo,0}};
    vkUpdateDescritorSets(device,2,writeDescriptorSet,0,0);
    return true;}
layout(local_size_x = 1,local_size_y =1,local_size_z =1) in;
layout(constant_id =0) const uint NUM_SAMPLES =1024u;
layout(set=0,binding=0) buffer SRC{float data[];} src;
layout(set=0,binding=1) buffer DST{float data[];} dst;
const uint BRDF_W =256;
const uint BRDF_H = 256;
const float PI = 3.14159226536;
void main(){
    vec2 uv;
    uv.x = float(gl_GlobalInvocationID.x) / float(BRDF_W);
    uv.y = float(gl_GlobalInvocationID.y)/float(BRDF_H);
    vec2 v =BRDF(uv.x,1.0-uv.y);
    uint offset = gl_GlobalInvocationID.y * BRDF_W + gl_GlobalInvocationID.x;
    dst.data[offset * 2 +0] = v.x;
    dst.data[offset * 2 +1] = v.y;}
vec2 hammersley2d(uint i,uint N){
    uint bits = ( i<<16u) | (i >> 16u);
    bits = ((bits&0x55555555u) << 1u) | ((bits&0xAAAAAAAAu) >> 1u);
    bits = ((bits&0x55555555u) << 2u) | ((bits&0xAAAAAAAAu) >> 2u);
    bits = ((bits&0x55555555u) << 4u) | ((bits&0xAAAAAAAAu) >> 4u);
    bits = ((bits&0x55555555u) << 8u) | ((bits&0xAAAAAAAAu) >> 8u);
    float rdi = float(bits) * 2.328306436538693e-10;
    return vec2(float(i)/float(N),rdi);}
float rendom(vec2 co){
    float a = 12.9898;
    float b = 78.233;
    float c = 43758.5453;
    float dt = dot(co.xy,vec2(a,b));
    float sn = mod(dt,PI);
    return fract(sin(sn)*c);}

    
    
