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

