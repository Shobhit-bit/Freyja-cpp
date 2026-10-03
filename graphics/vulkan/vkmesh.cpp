bool createtexturedVertexBuffer(VulkanRenderDevice& vkDev,const char* filename,VkBuffer* storageBuffer,VkDeviceMemory* storeageBufferMemory,sizet* vertexBufferSize,size_t* indexBufferSize){
    const siScene* scene = aiImportFile(filename,aiProcess_Triangulate);
    if(!scene || !scene->hasMeshes()){
        printf(Unable to load 5s\n",filename);
        exit(255);}
    const aiMesh* mesh = scene->mMeshes[0];
    struct VertexData{
        vec3 pos;
        vec2 tc;};
std::vector<VertexData> vertices;
for(unisigned i=0;i!=mesh->mNumVertices;i++){
    const aiVector3D v =mesh->mVertices[i];
    const aiVector3D t = mesh-> mTextureCoords[0][i];
    vertices.push_back({vec3(v.x,v.z,v.y),vec2(t.x,t.y)});}
    std::vector<unsigned int> indices;
    for(unsigned i=0;i!=mesh->mNumFaces;i++)
        for(unsignedj=0;j!=3;j++)
            indices.push_back(mesh->mFace[i].mIndices[j]);
    aiReleaseImport(scene);
    *vertexBufferSize = sizeof(vertexData)*vertices.size();
    *indexBufferSize = sizeof(unsigned int) * indices.size();
    VkDeviceSize bufferSize = *vertexBufferSize + *indexBufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    createBuffer(vkDev.device,vkDev.physicalDevice,bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERT_BIT,stagingBuffer,stagingMemory);
    void* data;
    vkMapMemory(vkDev.device,staginMemory,0,bufferSize,0,&data);
    memcpy(data,vertices.data(),*vertexBufferSize);
    memcpy((unsigned char *)data+*vertexBufferSize,indices.data(),*indexBufferSize);
    vkUnmapMemory(vkDev.device,stagingMemory);
    createBuffer(vkDev.device,vkDev.physicalDevice,bufferSize,VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,*storageBuffer,*storageBufferMemory);
    copyBuffer(vkDev.device,vkDev.commandPool,vkDev.graphicsQueue,stagingBuffer,*storageBuffer,bufferSize);
    vkDestroyBuffer(vkDev.device,stagingBuffer,nullptr);
    vkFreeMemory(vkDev.device,stagingMemory,nullptr);
    return true;}
#version 460
layout(location = 0) out vec1 fragColor;
layout(location =1) out vec2 uv;
layout(binding =0) uniform UniformBuffer{
    mat4 mvp;}
ubo;
struct VertexData{
    float x,y,z;
    float u,v;};
layout(binding =1)
    readonly buffer Vertices{VertexData data[];}
    in_Vertices;
layout(binding=2)
    readonly buffer Indices{uint data[];} in_Indices;
void main(){
    uint idx = in_Indices.data[gl_VertexIndex];
    VertexData vtx = in_Vertices.data[idx];
    vec3 pos =vec(vtx.x,vtx.y,vtx.z);
    gl_Position = ubo.mvp * vec4(pos,1.0);
    fragColor = pos;
    uv = vec2(vtx.u,vtx.v);}
#version 460
layout(triangles) in;
layout(triangle_strip,max_vertices=3) out;
layout(location=0) in vec3 color[];
layout(location=1) in vec2 uvs[];
layout(location=0) out vec3 fragColor;
layout(location=1) out vec3 baryCoords;
layout(location=2) out vec2 uv;
void main(){
    const vec3 bc[3] = vec3[](vec3(1.0,0.0,0.0),vec3(0.0,1.0,0.0),vec3(0.0,0.0,1.0));
    for(int i=0;i<3;i++){
        gl_Position = gl_in[i].gl_Position;
        fragColor = color[i];
        barycoords = bc[i];
        uv = uvs[i];
        EmitVertex();}
    EndPrimitive();}
#version 460
layout(location=0) in vec3 fragColor;
layout(location=1) in vec3 barycoords;
layout(location=2) in vec2 uv;
layout(location=0) out vec4 outColor;
layout(binding=3) uniform sampler2D texSampler;
float edgeFactor(float thickness){
    vec3 a3 =smoothstep(vec3(0.0),fwidth(barrycoords)* thickness,barycoords);
    return min(min(a3.x,a3.y),a3.z);}
void main(){
    outColor = vec4(mix(vex(0.0),texture(texSampler,uv).xyz,edgeFactor(1.0)),1.0);}
//descriptor
bool createDescriptionPool(VkDevice device,uint32_t imageCount,uint32_t uniformBufferCount,uint32_t storageBufferCount,uint32_t samplerCount,VkDescriptorPool* descPool){
    std::vector<VkDescriptorPoolSize poolSizes;
    if(uniformBufferCount)
        poolSizes.push_back(VkDscriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,.descriptorCount=imageCount*uniformBufferCount});
    if(storageBufferCount)
        poolSizes.push_back(VkDescriptorPoolSize{type = VK_DECRIPTOR_TYPE_STORAGE_BUFFER,.descriptorCount = imageCount * storageBufferCount});
    if(samplerCount)poolSizes.push_back(VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,.descriptorCount = imageCount*samplerCount});
    const VkDescriptorPoolCreateInfo pi={.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,.pNext = nullptr,.flags=0,.maxSets = static_cast<uint32_t>)imageCount),.poolSizeCount = static_cast<uint32_t>(imageCount),.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),.pPoolSizes = poolSizes.empty()? nullptr: poolSizes.data()};
    VK_CHECK(vkCreateDescriptorPool(device,&pi,nullptr,descPool));
    return true;}
bool createDescriptorSet(){
    const std::array<VkDescriptorSetLayoutBinding,4>bindings = {descriptorSetLayoutBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_DHADER_STAGE_VERTEX_BIT),descriptorSetLayoutBinding(1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(2,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(3,VK_DESCRIPTOR,TYPE,COMBINEDD_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT)};
    const VkDescriptorSetLayoutCreateInfo li = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,.pNext=nullptr,.flags =0,.bindingCount = static_cast<uint32_t>(binding_size()),.pBindings = bindings.data()};
    VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device,&li,nullptr,&vkState.descriptorSetLayout));
    std::vector<VkDescriptorSetLayout>
layouts(vkDev.swapchainImages.size(),vkState.descriptorSetLayout);
    VkDescriptorSetAllocateInfo ai = { .sType = VK_STUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,.pNext = nullptr,.descriptorPool = vkState.descriptorPool,.descriptorSetCount=static_cast<uint32_t>(vkDev.swapchainImages.size())),.pSetLayouts =layouts.data()};
    vkState.descriptorSets.resize(vkDev.swapchainImages.size());
    VK_CHECK(vkAllocateDescriptorSets(vkDev.device,&ai,vkState.descriptorSets.data());
    for (size_t i = 0;i<vkDev.swapchainImages.size();i++){
        VkDescriptorBufferInfo bufferInfo = {.buffer = vkState.uniformBuffers[i],.offset = 0,.range = sizeof(UniformBuffer)};
    VkDescriptorBufferInfo bufferInfo2 = {.buffer=vkState.storageBuffer,.offset=0,.range=vertexBufferSize};
    VkDescriptorBufferInfo bufferInfo3 = {.buffer=vkState.storageBuffer,.offset = vertexBufferSize,.range=indexBufferSize};
    VkDescriptorImageINfo imageInfo = {.sampler=vkState.textureSampler,.imageView = vkState.texture.imageView,.imageLaoyut = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
    std::array<VkWriteDescriptorSet,4> descriptorWrites = {VkWriteDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,.dstSet = vkState.descriptorSets[i],.dstBinding =0,.dstArrayElement=0,.descriptorCount=1,.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,pBufferInfo = &bufferInfo},
    VkWriteDescriptiorSet{.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,.dstset= vkState.descriptorSets[i],.dstBinding =1,.dstArrayElements =0,.descriptorCount=1,.descriptorType=VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,.pBufferInfo = &bufferInfo2},VkWriteDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,.dstset=vkState.descriptorSets[i],.dstBinding=2,.dstArrayElement=0,.descriptorCount=1,.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,.pBufferInfo = &bufferInfo3},VkWriteDescriptorSet{.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,.dstSet=vkState.descriptorSets[i],.dstBinding=3,.dstArrayElement=0,.descriptorCount=1,.descriptorType=VK_DESCRIPTOR_TYPR_COMBINED_IMAGE_SAMPLER,.pImageInfo = &imageInfo},};
vkUpdateDescriptionSets(vkDev.device,static_cast<uint32_t>(descriptorWrites.size()),descriptorWrites.data(),0,nullptr);}
return true;}
//shader module
struct shaderModule{
    std::vector<unsigned int> SPIRV;
    VkShaderModule shaderModule;};
VkResult createShaderModule(VkDevice device,ShaderModule* sm,const char* fileName){
    if(!compileShaderFile(fileName,*sm)) return VK_NOT_READY;
    const VkShaderModuleCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,.codeSize=shader->SPIRV.size() * sizeof(unsigned int),.pCode = shader->SPIRV.data()};
    return vkCreateShaderModule(device,&createInfo,nullptr,&sm->shaderModule);}


