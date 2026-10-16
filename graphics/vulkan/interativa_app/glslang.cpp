#include <fstream>
#include <sstream>
#include <string>
#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>
// ShaderModule is already defined in vkmesh.cpp (duplicate struct removed from here)
glslang_stage_t glslangShaderStageFromFileName(const char* fileName){
    const std::string s(fileName);
    auto ends = [&s](const char* e){ const std::string t(e); return s.size() >= t.size() && s.compare(s.size()-t.size(),t.size(),t) == 0;};
    if(ends(".vert")) return GLSLANG_STAGE_VERTEX;
    if(ends(".frag")) return GLSLANG_STAGE_FRAGMENT;
    if(ends(".geom")) return GLSLANG_STAGE_GEOMETRY;
    if(ends(".comp")) return GLSLANG_STAGE_COMPUTE;
    if(ends(".tesc")) return GLSLANG_STAGE_TESSCONTROL;
    if(ends(".tese")) return GLSLANG_STAGE_TESSEVALUATION;
    return GLSLANG_STAGE_VERTEX;}
std::string readShaderFile(const char* fileName){
    std::ifstream f(fileName);
    if(!f.is_open()){
        printf("Cannot open shader file [%s]\n",fileName);
        return std::string();}
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();}
void saveSPIRVBinaryFile(const char* filename,unsigned int* code,size_t size){
    FILE* f = fopen(filename,"wb");
    if(!f) return;
    fwrite(code,sizeof(unsigned int),size,f);
    fclose(f);}
size_t compileShader(glslang_stage_t stage,const char* shaderSource ,ShaderModule& shaderModule){
    const glslang_input_t input = {.language=GLSLANG_SOURCE_GLSL,.stage=stage,.client=GLSLANG_CLIENT_VULKAN,.client_version=GLSLANG_TARGET_VULKAN_1_1,
        .target_language = GLSLANG_TARGET_SPV,.target_language_version=GLSLANG_TARGET_SPV_1_3,.code=shaderSource,.default_version=100,.default_profile=GLSLANG_NO_PROFILE,
        .force_default_version_and_profile = false,.forward_compatible=false,.messages=GLSLANG_MSG_DEFAULT_BIT,.resource=glslang_default_resource(),};
    glslang_shader_t* shd=glslang_shader_create(&input);
    if(!glslang_shader_preprocess(shd,&input)){
        fprintf(stderr,"GLSL prepro failed");
        fprintf(stderr,"\n%s",glslang_shader_get_info_log(shd));
        fprintf(stderr,"\n%s",glslang_shader_get_info_debug_log(shd));
        fprintf(stderr,"code:\n%s",input.code);
        return 0;
    }
    if (!glslang_shader_parse(shd,&input)){
        fprintf(stderr,"GLSL parsing failed\n");
        fprintf(stderr,"\n%s",glslang_shader_get_info_log(shd));
        fprintf(stderr,"\n%s",glslang_shader_get_info_debug_log(shd));
        fprintf(stderr,"%s",glslang_shader_get_preprocessed_code(shd));
        return 0;}
    glslang_program_t* prog = glslang_program_create();
    glslang_program_add_shader(prog,shd);
    int msgs = GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT;
    if (!glslang_program_link(prog,msgs)){
        fprintf(stderr,"GLSL linking failed");
        fprintf(stderr,"\n%s",glslang_program_get_info_log(prog));
        fprintf(stderr,"\n%s",glslang_program_get_info_debug_log(prog));
        return 0;}
    glslang_program_SPIRV_generate(prog,stage);
    shaderModule.SPIRV.resize(glslang_program_SPIRV_get_size(prog));
    glslang_program_SPIRV_get(prog,shaderModule.SPIRV.data());
    const char* spirv_messages = glslang_program_SPIRV_get_messages(prog);
    if(spirv_messages)
        fprintf(stderr,"%s",spirv_messages);
    glslang_program_delete(prog);
    glslang_shader_delete(shd);
    return shaderModule.SPIRV.size();}
// returns bool to match the declaration in vkmesh.cpp
bool compileShaderFile(const char* file,ShaderModule& shaderModule){
    if (auto shaderSource = readShaderFile(file); !shaderSource.empty())
        return compileShader(glslangShaderStageFromFileName(file),shaderSource.c_str(),shaderModule) > 0;
    return false;}
void testShaderCompilation(const char* sourceFilename,const char* destFilename){
    ShaderModule shaderModule;
    if (!compileShaderFile(sourceFilename,shaderModule)) return;
    saveSPIRVBinaryFile(destFilename,shaderModule.SPIRV.data(),shaderModule.SPIRV.size());}
// The test main() was removed (it would clash with main() in vkdemof.cpp); call testShaderCompilation() yourself if needed.
// glslang_initialize_process()/finalize_process() are already called in vkdemof.cpp's main().
// Offline equivalent (shell, not C++):
//   glslangValidator -V110 --target-env spirv1.3 VK01.vert -o VK01.vrt.bin
//   glslangValidator -V110 --target-env spirv1.3 VK01.frag -o VK01.frg.binlslangValidator -V110 --target-env spirv1.3 VK01.frag -o VK01.frg.bin
