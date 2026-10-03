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

