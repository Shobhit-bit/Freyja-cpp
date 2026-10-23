std::deque<std::pair<uint32_t,uint32_t>> morphQueue = {{5,8},{5,8}};
float morphCoef = 0.0f;
float animationSpeed = 1.0f;
const uint32_T numU =1024;
const uint32_T numV = 1024;
struct MeshUniformBuffer{
    float time;
    uint32_t numU;
    uint32_t numV;
    float minU,maxU;
    float minV,maxV;
    uint32_t p1,p2;
    uint32_t q1,q2;
    float morph;
}ubo;
void generateIndices(uint32_t * indices){
    for(uint32_t j=0;j<numV -1;j++){
        for(uint32_t i=0;i<numU -1;i++){
            uint32_t offset = (j*(numU-1)+1)*6;
            uint32_t i1 = (j+0)*numU+(i+0);
            uint32_t i2 = (j+0)*numU+(i+1);
            uint32_t i3 = (j+1)*numU+(i+1);
            uint32_t i4 = (j+1)*numU+(i+0);
            indices[offset+0] = i1;
            indices[offset+1] = i2;
            indices[offset+2] = i4;
            indices[offset+3] = i2;
            indices[offset+4] = i3;
            indices[offset+5] = i4;}}}
void initMesh(){
    std::vector<uint32_t> indicesGen((numU-1)*(numV-1)*6);
    generateIndices(indicesGen.data());
    uint32_t vertexBufferSize = 12*sizeof(float)*numU*numV;
    uint32_t indexBufferSize = 6*sizeof(uint32_t)*(numU-1)*(numV-1);
    uint32_t bufferSize = vertexBufferSize + indexBufferSize;
    imgGen =std::make_unique<ComputedImage>(vkDev,"data/shader/VK04_compute_texture.comp",1024,1024,false);
    meshGen = std::make_unique<ComputedVertexBuffer>(vkDev,"data/shaders/VK04_compute_mesh.comp",indexBufferSize,sizeof(MeshUniformBuffer),12*sizeof(float),numU*numv);
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(vkDev.device,vkDev.physicalDevice,bufferSize,VK_BUFFER_USAGE_TRANFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,stagingBuffer,stagingBufferMemory);
    void* data = nullptr;
    vkMapMemory(vkDev.device,stagingBufferMemory,0,bufferSize,0,&data);
    memcpy((void*)((uint8_t*)data+vertexBufferSize),indicesGen.data(),indexBufferSize);
    vkUnmaoMemory(vkDev.device,stagingBufferMemory);
    copyBuffer(vkDev,stagingBuffer,meshGen->computedBuffer,bufferSize);
    vkDestroyBuffer(vkDev.device,stagingBuffer,nullptr);
    vkFreeMemory(vkDev.device,stagingBufferMemory,nullptr);
    vkFreeMemory(vkDev.device,stagingBufferMemory,nullptr);
    meshGen->fillComputeCommandBuffer();
    meshGen->submit();
    vkDeviceWaitIdle(vkDev.device);
    std::vector<const char*> shaders = { "data/shaders/VK04_render.vert","data/shaders/VK04_render.frag"};
    mesh = std::make_unique<ModelRender>(VkDev,true,meshGen->computeBuffer,meshGen->computedMemory,vertexBufferSize,indexBufferSize,imgGen->computed,imgGen->computedImageSampler,shadersColor,(uint32_t)sizeof(mat4),true,mesh->getDepthTexture(),false);}
#version 440
layout(local_size_x =2,local_size_y = 1,local_size_z =1) in;
struct VertexData{
    vec4 pos,tc,norm;
};
layout(binding=0) buffer VertexBuffer{
    VertexData vertices[];
}vbo;
layout (binding=1) uniform UniformBuffer{
    float time;
    uint numU,numV;
    float minU,maxU,minU,maxU;
    uint P1,P2,Q1,Q2;
    float morph;
}ubo;
x = r*cos(u)
y = r*sin(u)
z = -sin(v)
VertexData torusKnot(vec2 uv,vec2 pq){
    const float p = pq.x;
    const float q = pq.y;
    const float baseRadius = 5.0;
    const float segmentRadius = 3.0;
    const float tubeRadius = 0.5;
    float ct = cos(uv.x);
    float st = sin(uv.x);
    float qp = q/p;
    float qps = qp * segmentRadius;
    float arg = uv.x * qp;
    float sqp = sin(arg);
    float cpq = cos(arg);
    float BSQP = baseRadius + segmentRadius +cqp;
    float dxdt = -qps * sqp *ct -st *BSQP;
    float dydt = -qps * spq *st +ct *BSQP;
    float dzdt = qps * cpq;
    vec3 r = vec3(BSQP * ct,BSQP *st,segementRadius * sqp);
    vec3 drdt = vec3(dxdt,dydt,dzdt);
    vec3 v1 = normalize(cross(r,drdt));
    vec3 v2 = normalize(cross(v1,drdt));
    float cv = cos(uv.y);
    float sv = sin(uv.y);
    VertexData res;
    res.pos = vec4(r+tubeRadius*(v1*sv+v2*cv),1);
    res.norm = vec4(cross(v1*cv -v2 *sv,drdt),0);
    return res;}
mat3 rotY(float angle){
    float c = cos(angle),s=sin(angle);
    return mat3(c,0,-s,0,1,0,s,0,c);}
mat3 rotZ(float angle){
    float c = cos(angle),s = sin(angle);
    return mat3(c,-s,0,s,c,0,0,0,1);}
void main(){
    uint index = gl_GlobalInvocationID x;
    vec2 numUV = vec2(ubo.numU,ubo.numV);
    vec2 ij = vec2(float(index/ubo.numU),float(index%ubo.numU));
    const vec2 maxUV1 = 2.0*3.1415926 * vec2(ubo.p1,1.0);
    vec2 uv1 = ij * maxUV1 /(numUV - vec2(1));
    const vec2 maxUV2 = 2.0 * 3.1415926 * vec2(ubo.P2,1.0);
    vec2 uv2 = ij * maxUV2/ (numUV - vec2(1));
    mat3 modelMatrix = rotY(0.5 * ubo.time) * rotZ(0.5*ubo.time);
    VertexData v1 = torusKnot(uv1,vec2(ubo.P1,ubo.Q1));
    VertexData v2 = torusKnot(uv2,vec2(ubo.P2,ubo.Q2));
    vec3 pos = mix(v1.pos.xyz,v2.pos.xyz,ubo.morph);
    vec3 norm = mix(v1.norm.xyz,v2.norm.xyz,ubo.morph);
    VertexData vtx;
    vtx.pos = vec4(modelMatrix * pos,1);
    vtx.tc = vec4(ij/numUV,0,0);
    vtx.orm = vec4(modelMatrix * norm,0);
    vbo.vertices[index] = vtx;}

