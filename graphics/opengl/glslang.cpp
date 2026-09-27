struct ShaderModule{
    std::vector<unsigned int> SPIRV;
    VkShaderModule shaderModule;};
size_t compileShader(glSlang_stage_t stage,const char* shaderSource ,shaderModule& shaderMOdule){
    const glslang_input_t input = {.language=GLSLANG_SOURCE_GLSL,.stage=stage,.client=GLSLANG_CLIENT_VULKAN,.client_version=GLSLANG_TARGET_VULKAN_1_1,
        .target_language = GLSLANG_TARGET_SPV,.target_language_version=GLSLANG_TARGET_SPV_1_3,.code=shaderSource,.default_version=100,.default_profile=GLSLANG_NO_PROFILE,
        .force_default_version_and_profile = false,.forward_compatible=false,.messages=GLSLANG_MSG_DEAFULT_BIT,.resource=(const glslang_resource_t*) &glslang::DefaultTBuiltInResource,};
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
        fprintf(
