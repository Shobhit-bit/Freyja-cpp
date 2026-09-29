const aiScene* scene = aiImportFile("/home/arc/dev/engine/data/rubber_duck/scene.gltf",aiProcess_Triangulate);
struct VertxData{ vec3 pos; vec2 tc;};
const aiMesh* mesh = scene->mMeshes[0];
std::vector<VertexData> vertices;
for(unsigned i=0;i!=mesh->mNumVertices;i++){
    const aiVector3D v =mesh->mVertices[i];
    const aiVector3D t=mesh->mTextureCoords[0][i];
    vertices.push_back({.pos=vec3(v.x,v.z,v.y),.tc=vec2(t.x,t.y)});
}
std::vector<unsigned int> indices;
for(unsigned i =0;i!=mesh->mNumFaces;i++){
    for(unsigned j=0;j!=3;j++)
        indices.push_back(mesh->mFaces[i].mIndices[j]);
}
const size_t kSizeIndices=sizeof(unsigned int)*indices.size();
const size_t kSizeVertices=sizeof(VertexData)*vertices.size();
GLuint dataIndices;
glCreateBuffer(1,&dataIndices);
glNamedBufferStorage(dataIndices,kSizeIndices,indices.data(),0);
GLuint dataVertices;
glCreateBuffer(1,&dataVertices);
glNamedBufferStorage(dataVertices,kSizeVertices,vertices.data(),0);
GLuint vao;
glCreateVertexArrays(1,&vao);
glBindVertexArray(vao);
glVertexArrayElementBuffer(vao,dataIndices);
glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,dataVertices);
int w,h,comp;
const uint8_t* img = sstbi_load("/home/arc/dev/engine/data/rubber_duck/DuckCM.png",&w,&h,&comp,3);
GLuint tx;
glCreateTextures(GL_TEXTURE_2D,1,&tx);
glTexturePrarameteri(tx,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
glTexturePrarameteri(tx,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
glTextureStorage2D(tx,1,GL_RGB8,w,h);
glPixelStori(GL_UNPACK_ALIGNMENT,1);
glTextureSubImage2D(tx,0,0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,img);
glBIndTextures(0,1,&tx);
#version 460 core
layout(std140,binding=0)uniform PerFrameData{uniform mat4 MVP;};
struct Vertex{float p[3];float tc[2];};
layout(std430,binding=1)readonly buffer Vertices{Vertex in_Vertices[];};
vec3 getPosition(int i){
    return vec3(in_Vertices[i].p[0],in_Vertices[i].p[1],in_Vertices[i].p[2]);}
vec2 getTexCoord(int i){
    return vec2(in_Vertices[i].tc[0],in_Vertices[i].tc[1]);}
layout (location=0) out vec2 uv;
void main(){
    vec3 pos=getPosition(gl_VertexID);
    gl_position = MVP*vec4(pos,1.0);
    uv=getTexCoord(gl_VertexID);}

