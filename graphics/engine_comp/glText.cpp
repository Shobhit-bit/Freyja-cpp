GLTexture texAO(GL_TEXTURE_2D,"DamagedHaelmet/gltf/Default_AO.jpg");
GLTexture texEmmisive(GL_TEXTURE_2D,"DamagedHelmet/gltf/Default_emissive.jpg");
GLTexture texAlbedo(GL_TEXTURE_2D,"DamagedHelmet/glTF/Default_albedo.jpg");
GLTexture texMeR(GL_TEXTURE_2D,"DamagedHelmet/glTF/Default_metalRoughness.jpg");
GLTexture texNormal(GL_TEXTURE_2D,"DamagedHelmet/glTF/Default_normal.jpg");
const GLuint textures[] = {texAO.getHandle(),texEmissive.getHandle(),texAlbedo.getHandle(),texMeR.getHandle(),texNormal.getHandle()};
glBindTextures(0,sizeof(textures)/sizeof(GLuint),textures);
GLTexture envMap(GL_TEXTURE_CUBE_MAP,"data/piazza_bologni_1k.hdr");
GLTexture envMapIrradiance(GL_TEXTURE_CUBE_MAP,"data/piazza_bologni_1k_irrandance.hdr");
const GLuint envMaps[] = {envMap.getHandle(),envMapIrradiance.getHandle()};
glBindTextures(5,2,envMaps);
GLTexture brdfLUT(GL_TEXTURE_2D,"data/brdfLUT.ktx");
glBindTextureUnit(7,brdfLUT.getHandle());
#version 460 core
layout(std140,binding=0) uniform PerFrameData{
    mat4 view;
    mat4 proj;
    vec4 cameraPos;};
layout (loation=0) in vec2 tc;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 worldPos;
layout (location=0) out vec4 out_FragColor;
layout(binding =0)uniform sampler2D texAO;
