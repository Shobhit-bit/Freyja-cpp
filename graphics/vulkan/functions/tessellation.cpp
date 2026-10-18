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
    glAttachShader(

