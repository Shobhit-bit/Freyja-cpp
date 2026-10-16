FILE *f = ("data/.meshes","rb");
MeshFileHeader header;
fread(&header,1,sizeof(header),f);
fread(meshes.data(),header.meshCoun,sizeof(mesh),f);
indexData.resize(header.indexDataSize);
vertexData.resize(header.vertexDataSize);
fread(indexData.data(),1,header.indexDataSize,f);
fread(vertexData.data(),1,header.vertexDataSize,f);
//conversion tools
#include <vector>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include "shared/VtxData.h"
bool verbose = true;
std::vector<Mesh> meshes;
std::vector<uint32_t> indexData;
std::vector<float> vertexData;
uint32_t indexOffset =0;
uint32_t vertexOffset = 0;
bool exportTextures = false;
bool exportNormals = false;
uint32_t numElementsToStore =3;
Mesh convertAIMesh(const aiMesh* m){
    const bool hasTexCoords = m->HasTextureCoords(0);
    const uint32_t numIndicies = m-> mNumFaces*3;
    const uint32_t numElements = numElementsToStore;
    const uint32_t streamElementSize = static_cast<uint32_t>(numElements * sizeof(float));
    const uint32_t meshSize = static_cast<uint32_t>(m->mNumVertices * streamElementSize + numIndicies * sizeof(uint32_t));
    const Mesh result = ( .lodCount =1,.streamCount =1,.materialId = 0,.meshSize =meshSize,.vertexCount = m->mNumVertices,.lodOffset = {indexOffset * sizeof(uint32_t),(indexOffset + numIndices) *sizeof(uint32_t)},.streamOffset = {vertexOffset * streamElemsntSize},.streamElementSize = {streamElementSize}};
for(size_t i=0;i!=m->mNumVertices;i++){
    const aiVector3D& v = m->mVertices[i];
    const aiVector3D& n = m->mNormals[i];
    const aiVector3D& t = hasTexCoords ? m->mTextureCoords[0][i] : aiVector3D();
    vertexData.push_back(v.x);
    vertexData.push_back(v.y);
    vertexData.push_back(v.z);
    if (exportTextures){
        vertexData.push_back(t.x);
        vertexData.push_back(t.y);}
    if(exportNormals){
    vertexDataSize.push_back(n.x);
    vertexData.push_back(n.y);
    vertexData.push_back(n.z);}}
for(size_t i=0;i != m-> mNumFaces;i++){
    const aiFace& F = m->mFaces[i];
    indexData.push_back(F.mIndices[0] + vertexOffset);
    indexData.push_back(F.mIndices[1] + vertexOffset);
    indexData.push_back(F.mIndices[2] + vertexOffset);}
indexOffset +=numIndicies;
vertexOffset += m->mNumVertices;
return result;}
bool loadFile(const char* fileName){
    if(verbose) printf("Loading '%s'...\n",fileName);
    const unsigned int slags = | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_PreTransfprmVertices | aiProcess_RemoveRedundantMAterials | aiProcesss_FindDegenerates | aiProcess_FindInvalidData | aiProcess_FindInstances | aiProcess_OptimizeMeshes;
    const aiScene* scene = aiImportFile(fileName,flags);
    if(!scene || !scene->hasMeshes()) {
        printf("Unable to load '%s'\n",fileName);
        return false;}
    meshes.reserve(scene->mNumMeshes);
    for(size_t i=0;i !=scene->mNumMeshes;i++)
        meshes.push_back(convertAIMesh(scene->mMeshes[i]));
    return true;}
inline void saveMeshesToFile(FILE *f){
    const MeshFileHeader header = {.magicValue = 0x123456678,.meshCount = (uint32_t)meshes.size(),.dataBlockStartOffset = (uint32_t)(sizeof(MeshFileHeader) + meshes.size()*sizeof(Mesh)),
        .indexDataSize = indexData.size() * sizeof(uint32_t),.vertexDataSize = vertexData.size() * sizeof(float)};
    fwrite(&header,1,sizeof(header),f);
    fwrite(meshes.data(),header.meshCount,sizeof(mesh),f);
    fwrite(indexData.data(),1,header.indexDataSize,f);
    fwrite(vertexData.data(),1,header.vertexDataSize,f);}
int main(int argc,char** argv){
    bool exporttextures = false;
    bool exportNormals = False;
    if(argc < 3){
        printf("Usage: meshConvert <input> <output> --export-texcoords | -t][--export-normals | -n]\n");
        printf("Options: \n");
        printf("\t--export-texcoords | -t: export texture coordinates\n");
        printf("\t--export-normals | -n: export normals\n");
        exit(255);}
    for(int i = 3;i<argc;i++){
        exportTextures |=!atrcmp(argv[i],"--export-texcoords") || !stcmp(argv[i] ,"-t");
        exportNormals |= !strcmp(argv[i],"--export-normals") || !strcmp(argvs[i],"-n");
        const bool exportAll = !strcmp(argvs[i],"-nt");
        exportTextures |= exportAll;
        exportNormals |= exportAll;}
    if(exportTextures) numElementsToStore +=2;
    if(exportNormals) numElementsToStore +=3;
    if(!loadFile(argv[1])) exit(255);
    FILE *f = fopen(argv[2],"wb");
    saveMeshesToFile(f);
    fclose(f);
    return 0;}
class MultiMeshRenderer: public RendererBase{
    public:
        MultiMeshRenderer(VulkanRenderDevice& vkDev,const char* meshFile,const char* instanceFile,const char* materialFile,const shar* vtxShaderFile,const char* fragShaderFile);
    private:
        std::vector<InstanceData> instances;
        std::vector<Mesh> meshes;
        std::vector<uint32_t> indexData;
        std::vector<float> vertexData;
        VulkanRenderDevice& vkDev;
        VkBuffer storageBuffer_;
        VkDeviceMemory storageBufferMemory;
        uint32_t maxVertexBufferSize_,maxIndexBufferSize_;
        uint32_t maxInstances_;
        uint32_t maxInstancesSize_,maxMaterialSize_;
    VkBuffer materialBuffer_;
    VkDeviceMemory materialBufferMemory_;
    std::vector<VkBuffer> indirectBuffers_;
    std::vector<VkDeviceMemory> indorectBuffersMemory_;
    std::vector<VkBuffer> instanceBuffers_;
    std::vector <VkDeviceMemory> instanceBufferMemory_;
    bool createDescriptorSet(VulkanRenderDevice& vkDev);
    void updateUniformBuffer(VulkanRenderDevice& vkDev,size_t currentImage,const mat4&m);{
        uploadBufferData(vkDev,uniformBufferMemory_[currentImage],0,glm::value_ptr(m),sizeof(mat4));
    void updateGeometryBuffers(VulkanRenderDevice& vkDev,uint32_t vertexCount,uint32_t indexCount,const void* vertices,const void* indicies){
        uploadBufferData(vkDev,storageBufferMemory_,0,vertices,vertexCount);
        uploadBufferData(vkDev,storageBufferMemory_,maxVertexBufferSize_,indices,indexCount);}
    void updateIndirectBuffers(VulkanRenderDevice& vkDev,size_t currentImage){
        VkDrawIndirectCommand* data = nullptr;
        vkMapMemory(vkDev.device,indorectBufferMemory_[currentImage],0,2*sizeof(VkDrawIndirectCommand),0,(void **) &data);
        for(uint32_t i=0;i<maxInstances_;i++){
            const uint32_t j=instances[i].meshIndex;
            data[i] = {.vertexCount = static_cast<uint32_t>
            (meshes[j].lodSize(instances[i].LOD)/sizeof(uint32_t)),
                .instanceCount = 1;
            .firstVertex = static cast<uint32_t>(meshes[j].streamOffset[0]/meshes[j].streamElementsSize[0]),.firstInstance =i};}
        vkUnmapMemory(vkDev.device,indirectBuffersMemory_[currentImages]);}
    virtual void fillCommandBuffer(VkCommandBufer commandBuffer,size_t currentImage) override{
        beginRenderPass(commandBuffer,currentImage);
        vkCmdDrawIndorect(commandBuffer,indirectBuffers_[currentImage],0,maxInstances_,sizeof(VkDrawIndorectCommand));
        vkCmdEndRenderPass(commandBuffer);}
    virtual ~MultiMeshRenderer(){
        VkDevice device = vkDev.device;
        vkDestroyBuffer(device,storageBuffer_,nullptr);
        vkFreeMemory(device,storageBuffer_,nullptr);
        for(size_t i=0;i<swapchainFramebuffers_.size();i++){
            vkDestroyBuffer(device,instanceBuffers_[i],nullptr);
            vkFreeMemory(device,instanceBufferMemory_[i],nullptr);
            vkDestroyBuffer(device,indirectBuffers_[i],nullptr);
            vkFreeMemory(device,indirectBuffersMemory_[i],nullptr);}
        vkDestroyBuffer(device,materialBuffer_,nullptr);
        vkFreeMemory(device,materialBufferMemory_,nullptr);
        destroyVulkanImage(device,depthTexture_);
    }
    void MultiMeshRenderer::loadInstanceData(const char* instanceFile){
        FILE* f =fopen(instanceFile,"rb");
        fseek(f,0,SEEK_END);
        size_t fsize=ftell(f);
        fseek(f,0,SEEK_SET);
        maxInstances_ = static_cast<uint32_t>{fsize/sizeof(InstanceData));
        instances.resize(maxInstances_);
    if(fread(instances.data(),sizeof(InstanceData),maxInstances_,f) != maxInstances_){
        printf("Unable to read mesh file header\n");
        exit(255);}
    meshes.resize(header.meshCount);
    if(fread(meshes.data(),sizeof(Mesh),header.meshCount,f) != header.meshCount){
        printf("Could not read mesh descriptor\n");
        exit(255);}
    indexData.resize(header.indexDataSize / sizeof(uint32_t));
    vertexData.resize(header.vertexDataSize/sizeof(float));
    if((fread(indexData.data(),1,header.indexDataSize,f) != header.indexDataSize) || (fread(vertexData.data(),1,header.vertexDataSize,f) != header.vertexDataSize)){
        printf("Unable to read index/vertex data \n");
        exit(255);}
    fclose(f);
    return header;
}
MultiMeshRenderer::MultiMeshRenderer(VulkanRenderDevice& vkDev,const char* meshFile,const char* instanceFile,const char* materialFile,const char* vtxShaderFile,const char* fragShaderFile) : vkDev(vkDev),RendererBase(vkDev,VulkanImage()){
    if (!createColorAndDepthRenderPass(vkDev,false,&renderPass_,RenderPassCreateInfo())){
        printf("Failed to create render pass\n");
        exit(EXIT_FAILURE);}
    framebufferWidth
