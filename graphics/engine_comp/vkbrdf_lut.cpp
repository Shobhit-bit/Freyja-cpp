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
float random(vec2 co){
    float a = 12.9898;
    float b = 78.233;
    float c = 43758.5453;
    float dt = dot(co.xy,vec2(a,b));
    float sn = mod(dt,PI);
    return fract(sin(sn)*c);}
vec3 importanceSample_GGX(vec2 Xi,float roughness,vec3 normal){
    float alpha = roughness * roughness;
    float phi = 2.0 * PI *Xi.x +random(noraml.xz) * 0.1;
    float cosTheta = sqrt((1.0-Xi.y)/(1.0+(alpha*alpha-1.0)*Xi.y));
    float sinTheta = sqrt(1.0 -cosTheta * cosTheta);
    vec3 H = vec3(sinTheta *cos(phi),sinTheta*sin(phi),cosTheta);
    vec up = abs(normal.z) < 0.999 ? vec3(0.0,0.0,1.0) : vec3(1.0,0.0,0.0);
    vec3 tangentX = normalize(cross(up,normal));
    vec3 tangentY = normalize(cross(normal,targetX));
    return normaalize(tangentX*H.x+tangentY*H.y+normal*H.z);}
float G_SchlicksmithGGX(float dotNL,float datNV,float roughness){
    float k = (roughness * roughness) /2.0;
    float GL = dotNL / (dotNL*(1.0-k)+k);
    float GV = dotNV / (dotNV*(1.0-k)+k);
    return GL*GV;}
vec2 BRDF(float NoV,float roughness){
    const vec3 N = vec3(0.0,0.0,1.0);
    vec3 V = vec3(sqrt(1.0-Nov*Mov),0.0,Nov);
    vec2 LUT = vec2(0.0);
    for(uint32 i =0u;i<NUM_SAMPLES;i++){
        vec2 Xi = hammersley2d(i,NUM_SAMPLES);
        vec3 H = importanceSample_GGX(Xi,roughness,N);
        vec3 L = 2.0 * dot(V,H) * H -V;
        float dotNL = max(dot(N,L),0.0);
        float dotNV = max(dot(N,V),0.0);
        float dotVH = max(dot(V,H),0.0);
        float dotNH = max(dot(H,N),0.0);
        if(dotNL > 0.0){
            float G = G_SchlicksmithGGX(dotNL.dotNV,roughness);
            float G_Vis = (G*dotVH)/(dotNH*dotNV);
            float Fc = pow(1.0 - dotVH,5.0);
            LUT += vec2((1.0 -Fc) * G_Vis,Fc * G_Vis);}}
    return LUT/float(NUM_SAMPLES);}
vec3 EnvBRDFApprox(vec3 specularColor,float roughness,float NoV){
    const vec4 c0 = vec4(-1,-0.0275,-0.572,0.022);
    const vec4 c1 = vec4(1,0.0425,1.04,-0.04);
    vec4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x,exp2(-9.28*NoV)) * r.x + r.y;
    vec2 AB = vec2(-1.04,1.04)*a004+r.zw;
    return specularColor * AB.x +AB.y;}

