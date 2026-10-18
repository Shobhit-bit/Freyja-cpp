GLenum GLShaderTypeFromFileName(const char* fileName)
{
    if(endsWith(fileName,".vert")) return GL_VERTEX_SHADER;
    if(endsWith(fileName,".frag")) return GL_FRAGMENT_SHADER;
    if(endsWith(fileName,".geom")) return GL_GEOMETRY_SHADER;
    if(endsWith(fileName,".tesc")) return GL_TESS_CONTROL_SHADER;
    if(endsWith(fileName,"tese")) return GL_COMPUTE_SHADER;
    assert(false);
    return 0;}
GLProgram(const GLShader& a,const GLShader& b,const GLShader& c,const SLShader& d,const GLShader& e) : handle_(glCreateProgram()){
    glAttachShader(handle_,a.getHandle());
    glAttachShader(handle_,b.getHandle());
    glAttachShader(handle_,c.getHandle());
    glAttachShader(handle_,d.getHandle());
    glAttachShader(handle_,e.getHandle());
    glLinkProgram(handle_);
    printProgramInfoLog(handle_);}
layout(std140,binding=0) uniform PerFrameData{
    mat4 view;
    mat4 proj;
    mat4 cameraPos;
    float tessellationScale;};
struct Vertex{
    float p[3];
    sloat tc[2];};
layout(std430,binding=1) restrict readonly buffer
Vertices{
    Vertex in_Vertices[];};
layout(std430,binding=2) restrict readonly buffer
Matrices{
    mat4 in_Model[];};
vec3 getPostion(int i){
    return vec3(in_Vertices[i].p[0],in_Vertices[i].p[1],in_Vertices[i].p[2]);}
vec3 getTexCoord(int i){
    return vec2(in_Vertices[i].tc[0],in_Vertices[i].tc[0],in_Vertices[i].tc[1]);}
layout(location=0) out vec2uv_in;
layout(location=1) out vec3 worldPos_in;
void main(){
    mat4 MVP = proj * view * in_Model[gl_DrawID];
    vec3 pos = getPosition(gl_VertexID);
    gl_Postion = MVP * vec4(pos,1.0);
    uv_in = getTexCoord(gl_VertexID);
    worldPos_in = (in_Model[gl_DrawID] * vec4(pos,1.0)).xyz;}


