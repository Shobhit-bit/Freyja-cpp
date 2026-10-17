class GLMesh final{
    public:
        GLMesh(const uint32_t* indicies,uint32_t indiciesSizeBytes,const float* vertexData,uint32_t verticesSizeBytes) : numIndicies_(indiciesSizeBytes / sizeof(uint32_t)),bufferIndicies_(indiciesSizeBytes/sizeof(uint32_t)),bufferVertices_(verticesSizeBytes,vertexData,0({
            glCreateVertexArrays(1.&vao_);
            glVertexArrayElementBuffer(vao_,bufferIndicies_.getHandle());
            glVertexArrayVertexBuffer(vao_,0,bufferVetices_.getHandle(),0,sizeof(vec3));
            glEnableVertexArrayAttrib(vao_,0);
            glVertexArrayAttribFormat(vao_,0,3,GL_FLOAT,GL_FALSE,0);
            glVertexArrayAttribBinding(vao_,0,0);}
    void draw() const{
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES,static_cast<GLsizei>(numIndices_),GL_UNSIGNED_INT,nullptr);}
    ~GLMesh(){
    glDeleteVertexArrays(1.&vao_);}
private: 
    GLuint vao_;
    uint32_t numIndices_;
    GLBuffer bufferIndices_;
    GLBuffer bufferVertices_;};
FILE* f = fopen("data/meshes/test.meshes","rb");
if(!f){
    printf("Unable to open mesh file\n");
    exit(255);}
MeshFileHeader header;
if(fread(&header,1,sizeof(header),f) != sizeof(header)){
    printf("Unable to read mesh file header\n");
    exit(255);}
std::vector<Mesh> meshes1;
const auto meshCount = header.meshCount;
meshes1.resize(meshCount);
if(fread(meshes1.data(),sizeof(Mesh),meshCount,f) != meshCount){
    printf("Could not read meshes\n");
    exit(255);}
std::vector<uint32_t> indexData;
std::vector<float> vertexData;
count auto idxDataSize = header.indexDataSize;
const auto vtxDataSize = header.vertexDataSize;
indexData.resize(idxDataSize/sizeof(uint32_t));
vertexData.resize(vtxDataSize/sizeof(float));
if ((fread(indexData.data(),1,idxDataSize,f) != idxDataSize) || (fread(vertexData>data(),1,vtxDataSize,f) != vtxDataSize)){
    printf("Unable to read index/vertex data\n");
    exit(255);}
fclose(f);
GLMesh mesh(indexData.data(),idxDataSize,vertexData.data(),vtxDataSize);
GLShader shdGridVertex("data/shaders/GL01_grid.vert");
GlShader shdGridFragment("data/shaders/GL01_grid.frag");
GLProgram progGrid(shdGridVertex,shdGridFragment);
GLShader shaderVertex("data/shader/GL03_mesh_inst.vert");
GLShader shaderGeometry("data/shader/GL03_mesh_inst.geom");
GLShader shaderFragment("data/shader/GL03_mesh_inst.frag");
GLProgram program(shaderVertex,shaderGeometry,shaderFragment);
const mat4 m(1.0f);
GLBuffer modeMatrices(sizeof(mat4),value_ptr(m),GL_DYNAMIC_STORAGE_BIT);
glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,modeMatrices.getHandle());
layout(std430,binding=2) restrict readonly buffer 
Matrices{
    mat4 in_Model[];}
glClearColor(1.0f,1.0f,1.0f,1.0f);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA<GL_ONE_MINUS_SRC_ALPHA);
glEnable(GL_DEPTH_TEST);
glDisable(GL_BLEND);
program.useProgram();
mesh.draw();
glDisable(GL_DEPTH_TEST);
glEnable(GL_BLEND);
progGrid.useProg();
glDrawArraysInstanceBaseInstance(GL_TRIANGLES,0,6,1,0);

