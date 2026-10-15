#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
bool createTexturedVertexBuffer(VulkanRenderDevice& vkDev,const char* filename,VkBuffer* storageBuffer,VkDeviceMemory* storageBufferMemory,size_t* vertexBufferSize,size_t* indexBufferSize){
    const aiScene* scene = aiImportFile(filename,aiProcess_Triangulate);
    if(!scene || !scene->HasMeshes()){
        printf("Unable to load %s\n",filename);
        exit(255);}
    const aiMesh* mesh = scene->mMeshes[0];
    struct VertexData{
        glm::vec3 pos;
        glm::vec2 tc;};
    std::vector<VertexData> vertices;
    for(unsigned i=0;i!=mesh->mNumVertices;i++){
        const aiVector3D v =mesh->mVertices[i];
        const aiVector3D t = mesh->mTextureCoords[0][i];
        vertices.push_back({glm::vec3(v.x,v.z,v.y),glm::vec2(t.x,t.y)});}
    std::vector<unsigned int> indices;
    for(unsigned i=0;i!=mesh->mNumFaces;i++)
        for(unsigned j=0;j!=3;j++)
            indices.push_back(mesh->mFaces[i].mIndices[j]);
    aiReleaseImport(scene);
    *vertexBufferSize = sizeof(VertexData)*vertices.size();
    *indexBufferSize = sizeof(unsigned int) * indices.size();
    VkDeviceSize bufferSize = *vertexBufferSize + *indexBufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    createBuffer(vkDev.device,vkDev.physicalDevice,bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,stagingBuffer,stagingMemory);
    void* data;
    vkMapMemory(vkDev.device,stagingMemory,0,bufferSize,0,&data);
    memcpy(data,vertices.data(),*vertexBufferSize);
    memcpy((unsigned char *)data+*vertexBufferSize,indices.data(),*indexBufferSize);
    vkUnmapMemory(vkDev.device,stagingMemory);
    createBuffer(vkDev.device,vkDev.physicalDevice,bufferSize,VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,*storageBuffer,*storageBufferMemory);
    copyBuffer(vkDev,stagingBuffer,*storageBuffer,bufferSize);
    vkDestroyBuffer(vkDev.device,stagingBuffer,nullptr);
    vkFreeMemory(vkDev.device,stagingMemory,nullptr);
    return true;}
// The GLSL shaders that were pasted here are not C++. They are now real files:
//   data/shaders/chapter04/VK02.vert, VK02.geom, VK02.frag
//descriptor
bool createDescriptorPool(VulkanRenderDevice& vkDev,uint32_t uniformBufferCount,uint32_t storageBufferCount,uint32_t samplerCount,VkDescriptorPool* descPool){
    const uint32_t imageCount = static_cast<uint32_t>(vkDev.swapchainImages.size());
    std::vector<VkDescriptorPoolSize> poolSizes;
    if(uniformBufferCount)
        poolSizes.push_back(VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,.descriptorCount=imageCount*uniformBufferCount});
    if(storageBufferCount)
        poolSizes.push_back(VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,.descriptorCount = imageCount * storageBufferCount});
    if(samplerCount)poolSizes.push_back(VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,.descriptorCount = imageCount*samplerCount});
    const VkDescriptorPoolCreateInfo pi={.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,.pNext = nullptr,.flags=0,.maxSets = imageCount,.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),.pPoolSizes = poolSizes.empty()? nullptr: poolSizes.data()};
    VK_CHECK(vkCreateDescriptorPool(vkDev.device,&pi,nullptr,descPool));
    return true;}
// descriptor helpers used by all the renderers (replace the old global createDescriptorSet() that used vkState)
VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(uint32_t binding,VkDescriptorType descriptorType,VkShaderStageFlags stageFlags,uint32_t descriptorCount = 1){
    return VkDescriptorSetLayoutBinding{.binding = binding,.descriptorType = descriptorType,.descriptorCount = descriptorCount,.stageFlags = stageFlags,.pImmutableSamplers = nullptr};}
VkWriteDescriptorSet bufferWriteDescriptorSet(VkDescriptorSet ds,const VkDescriptorBufferInfo* bi,uint32_t bindIdx,VkDescriptorType dType){
    return VkWriteDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,.pNext = nullptr,.dstSet = ds,.dstBinding = bindIdx,.dstArrayElement = 0,.descriptorCount = 1,.descriptorType = dType,.pImageInfo = nullptr,.pBufferInfo = bi,.pTexelBufferView = nullptr};}
VkWriteDescriptorSet imageWriteDescriptorSet(VkDescriptorSet ds,const VkDescriptorImageInfo* ii,uint32_t bindIdx){
    return VkWriteDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,.pNext = nullptr,.dstSet = ds,.dstBinding = bindIdx,.dstArrayElement = 0,.descriptorCount = 1,.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,.pImageInfo = ii,.pBufferInfo = nullptr,.pTexelBufferView = nullptr};}
//shader module
struct ShaderModule{
    std::vector<unsigned int> SPIRV;
    VkShaderModule shaderModule;};
bool compileShaderFile(const char* fileName,ShaderModule& shaderModule); // glslang GLSL->SPIR-V, implement as in the book's Shaders.cpp
VkResult createShaderModule(VkDevice device,ShaderModule* sm,const char* fileName){
    if(!compileShaderFile(fileName,*sm)) return VK_NOT_READY;
    const VkShaderModuleCreateInfo createInfo = {.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,.codeSize=sm->SPIRV.size() * sizeof(unsigned int),.pCode = sm->SPIRV.data()};
    return vkCreateShaderModule(device,&createInfo,nullptr,&sm->shaderModule);}
// (the three top-level VK_CHECK(createShaderModule(...)) lines were removed: createGraphicsPipeline() in vkinit.cpp does this)
