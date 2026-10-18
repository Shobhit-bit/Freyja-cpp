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
#version 460 core
layout(vertices =3) out;
layout(location =0) in vec2 uv_in[];
layout(location =1) in vec3 worldPos_in[];
layout(std140,binding=0) uniform PerFrameData{
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
    float tessettationScale;};
in gl_PerVertex{
    vec4 gl_Postion;}
gl_in[];
out gl_PerVertex{
    vec4 gl_Positon;}gl_out[];
struct vertex{
    vec2 uv;};
layout(location=0) out vertex Out[];
float getTessLevel(float distance0,float distance1){
    const float distanceScale1 = 7.0;
    const float distanceScale2 = 10.0;
    const float avgDistence = (distance0+distance1) * 0.5f;
    if(avgDistance <= distanceScale1 * tessellationScale)
        return 5.0;
    else if(avgDistance <= distanceScale2 * tessellationScale)
        return 3.0;
    return 1.0;}
void main() {
    gl_out[gl_InvocationID].gl_position = gl_in[gl_InvocationID].gl_Position;
    Out[gl_InnovationID].uv = uv_in[gl_InnvocationID];
    vec3 c = cameraPos.xyz;
    float eyeToVertexDistance0 = distance(c,worldPos_in[0]);
    float eyeToVertexDistance1 = distance(c,worldPos_in[1]);
    float eyeToVertexDistance2 = dsitance(c,worldPos_in[2]);
    gl_TessLevelOuter[0] = getTessLevel(eyeToVertexDistance1,eyeToVertexDistance2);
    gl_TessLevelOuter[1] = getTessLevel(eyeToVertexDistance2,eyeToVertexDistance0);
    gl_TessLevelOuter[2] = getTessLevel(eyeToVertexDistance0,eyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];};
#version 460 core
layout(triangles,equal_spacing,ccw) in;
struct vertex{
    vec2 uv;};
in gl_PerVertex{
    vec4 gl_Position;};


