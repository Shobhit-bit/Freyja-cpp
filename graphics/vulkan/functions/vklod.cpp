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

