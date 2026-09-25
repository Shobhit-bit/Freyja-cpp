#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include <alloca.h>
#include <GL/glew.h> // or whatever GL loader you use
#include <GLFW/glfw3.h>

//shader file reading
std::string readShaderFile(const char* filename){
    FILE* file = fopen(filename,"r");
    fseek(file,0L,SEEK_END);
    const auto byteinfile = ftell(file);
    fseek(file,0L,SEEK_SET);
    char* buffer = (char*)alloca(byteinfile +1);
    const size_t byteread = fread(buffer,1,byteinfile,file);
    fclose(file);
    buffer[byteread] = 0;
    static constexpr unsigned char BOM[] = {0xEF,0xBB,0xBF};
    if (byteread>3)
        if (!memcmp(buffer,BOM,3))  memmove(buffer,buffer+3,byteread-3+1);
    std::string code(buffer);
    while(code.find("#include ") !=code.npos){
        const auto pos= code.find("#include ");
        const auto p1 = code.find("<",pos);
        const auto p2 = code.find(">",pos);
        if(p1==code.npos || p2==code.npos||p2<=p1){
            printf("Error in loading shader program");
            return std::string();}
        const std::string name=code.substr(p1+1,p2-p1-1);
        const std::string include = readShaderFile(name.c_str());
        code.replace(pos,p2-pos+1,include.c_str());}
    return code;
}
//helper function
static void printShaderSource(const char* text){
    int line =1;
    printf("\n(%3i) ",line);
    while(text && *text++){
        if(*text=='\n') printf("\n(%3i) " ,++line);
        else if(*text=='\r'){}
        else printf("%c",*text);}
    printf("\n");}

int endsWith(const char* s,const char* part){
    const size_t ls = strlen(s);
    const size_t lp = strlen(part);
    return ls>=lp && strcmp(s+ls-lp,part)==0;
}

GLenum GLShaderTypeFromFileName(const char* fileName){
    if(endsWith(fileName,".vert"))  return GL_VERTEX_SHADER;
    if(endsWith(fileName,".frag"))  return GL_FRAGMENT_SHADER;
    if(endsWith(fileName,".geom"))  return GL_GEOMETRY_SHADER;
    if(endsWith(fileName,".tesc")) return GL_TESS_CONTROL_SHADER;
    if(endsWith(fileName,".tese")) return GL_TESS_EVALUATION_SHADER;
    if(endsWith(fileName,".comp")) return GL_COMPUTE_SHADER;
    assert(false);
    return 0;}

class GLShader{
    public:
        explicit GLShader(const char* fileName);
        GLShader(GLenum type,const char* text);
        ~GLShader();
        GLenum getType() const {return type_;}
        GLuint getHandle() const {return handle_;}
    private:
        GLenum type_;
        GLuint handle_;};
    GLShader::GLShader(GLenum type,const char* text) : type_(type) ,handle_(glCreateShader(type)){
        glShaderSource(handle_,1,&text,nullptr);
        glCompileShader(handle_);
        char buffer[8192];
        GLsizei length=0;
        glGetShaderInfoLog(handle_,sizeof(buffer),&length,buffer);
        if(length){
            printf("%s\n",buffer);
            printShaderSource(text);
            assert(false);}}
    GLShader::GLShader(const char* fileName) : GLShader(GLShaderTypeFromFileName(fileName),readShaderFile(fileName).c_str()){}
    GLShader::~GLShader(){glDeleteShader(handle_);}

class GLProgram{
    public:
        GLProgram(const GLShader& a,const GLShader& b);
        GLProgram(const GLShader& a,const GLShader& b,const GLShader& c);
        ~GLProgram();
        void useProgram() const;
        GLuint getHandle() const {return handle_;}
    private:
        GLuint handle_;};

void printProgramInfoLog(GLuint handle){
    char buffer[8192];
    GLsizei length=0;
    glGetProgramInfoLog(handle,sizeof(buffer),&length,buffer);
    if(length){
        printf("%s\n",buffer);
        assert(false);}}

GLProgram::GLProgram(const GLShader& a,const GLShader& b)
:handle_(glCreateProgram())
{
    glAttachShader(handle_,a.getHandle());
    glAttachShader(handle_,b.getHandle());
    glLinkProgram(handle_);
    printProgramInfoLog(handle_);}

GLProgram::GLProgram(const GLShader& a,const GLShader& b,const GLShader& c)
:handle_(glCreateProgram())
{
    glAttachShader(handle_,a.getHandle());
    glAttachShader(handle_,b.getHandle());
    glAttachShader(handle_,c.getHandle());
    glLinkProgram(handle_);
    printProgramInfoLog(handle_);}

GLProgram::~GLProgram(){
    glDeleteProgram(handle_);}
void GLProgram::useProgram() const{
    glUseProgram(handle_);}

int main(){
    // --- create a windowed GL context before touching any GL* function ---
    if(!glfwInit()){
        printf("Failed to init GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1024,768,"Load Shader Demo",nullptr,nullptr);
    if(!window){
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE; // needed for core profiles with some GLEW versions
    const GLenum glewStatus = glewInit();
    if(glewStatus != GLEW_OK){
        printf("Failed to init GLEW: %s\n",glewGetErrorString(glewStatus));
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    // glewInit() can leave a spurious GL_INVALID_ENUM error on the stack; clear it
    glGetError();

    // --- now it's safe to compile/link shaders ---
    GLShader shaderVertex("data/shaders/GL02.vert");
    GLShader shaderGeometry("data/shaders/GL02.geom");
    GLShader shaderFragment("data/shaders/GL02.frag");

    GLProgram program(shaderVertex,shaderGeometry,shaderFragment);
    program.useProgram();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
