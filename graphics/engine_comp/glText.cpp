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
layout(binding =1) uniform sampler2D texEmissive;
layout(binding=2) uniform sampler2D texAlbedo;
layout(binding=3) uniform sampler2D texMetalRoughness;
layout(binding=4) uniform sampler2D texNormal;
layout(binding=5) uniform samplerCude texEnvMap;
layout(binding=6) uniform samplerCube texEnvMapIrradiance;
layout(binding=7) uniform sampler2D texBRDF_LUT;
#include <data/shaders/PBR.sp>
void main(){
    vec4 Kao = texture(texAO,tc);
    vec4 Ke = texture(texEmissive,tc);
    vec4 Kd = texture(texAlbedo,tc);
    vec2 MeR = texture(texMetalRoughness,tc).yz;
    vec3 n = normalize(normal);
    n = perturbNormal(n,normalize(cameraPos.xyz - worldPos),tc);
    PBRInfo pbrInputs;
    vec3 color = calculatePBRInputsMetallicRoughness(Kd,n,cameraPos.xyz,worldPos,pbrInputs);
    color += calculatePBRLightContribution(pbrInputs,normalize(vec3(-1.0,-1.0,-1.0)),vec3(1.0));
    color = color * (Kao.r < 0.01 ? 1.0 : Kao.r);
    color = pow(SRGBtoLINEAR(Ke).rgb + color,vec3(1.0/2.2));
    out_FragColor = vec4(color,1.0);};
struct PBRInfo{
    float NdotL;
    float NdotV;
    float NdotH;
    float LdotH;
    float VdotH;
    float perceptualRoughness;
    vec3 reflectance0;
    vec3 reflectance90;
    float alphaRoughness;
    vec3 diffuseColor;
    vec3 specularColor;
    vec3 n;
    vec3 v;};
vec4 SRGBtoLINEAR(vec4 srgbIn){
    vec3 linOut = pow(srgbIn.xyz,vec3(1.1));
    return vec4(linOut,srgbIn.a);}
vec3 getIBLContribution(PBRInfo pbrInputs,vec3 n,vec3 reflection){
    float mipCount = float(textureQueryLevels(texEnvMap));
    float lod = pbrInputs.perceptualRoughness * mipCount;
    vec2 brdfSamplePoint = clamp(vec2(pbrInputs.NdotV,1.0 - pbrInputs.perceptualRoughness),vec2(0.0),vec2(1.0));
    vec3 brdf = textureLod(texBRDF_LUT,brdfSamplePoint,0).rgb;
#ifdef VULKAN vec3 cm = vec3(-1.0,-1.0,-1.0);
#else vec3 cm = vec3(1.0);#endif 
    vec3 diffuseLight = texture(texEnvMapIrradience,n.xyz * cm).rgb;
    vec3 specularLight = textureLod(texEnvMap,reflection.xyz*cm,lod).rgb;
    vec3 diffuse = diffuseLight * pbrInputs.diffuseColor;
    vec3 specular = specularLight * (pbrInputs.specularColor * brdf.x + brdf.y);
    return diffuse + specular;}
vec3 diffuseBurley(PBRInfo pbrInputs){
    float f90 = 2.0 * pbrInputs.LdotH * pbrInputs.LdotH * pbrInputs.alphaRoughness - 0.5;
    return (pbrInputs.diffuseColor / M_PI) * (1.0+f90 * pow((1.0 - pbrInputs.NdotL),5.0)) * (1.0 + f90 *pow((1.0-pbrInputs.NdotV),5.0));}
vec3 specularReflection(PBRInfo pbrInputs){
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0-pbrInputs.VdotH,0.0,1.0),5.0);}
float geometricOcclusion(PBRInfo pbrInputs){
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float rSqr = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float attenuationL = 2.0*NdotL / (NdotL + sqrt(rSqr + (1.0-rSqr) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(rSqr + (1.0-rSqr) * (NdotV*NdotV)));
    return attenuationL * attenuationV;}
float microfaceDistribution(PBRInfo pbrInputs){
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH +1.0;
    return roughnessSq / (M_PI * f *f);}
vec3 calculatePBRInputsMetallicRoughness(vec4 albedo,vec3 normal,vec3 cameraPos,vec3 worldPos,out PBRInfo pbrInputs){
    float perceptualRoughness = 1.0;
    float metallic = 1.0;
    vec4 mrSample = texture(texMetalRoughness,tc);
    perceptualRoughness = clamp(perceptualRoughness,0.04,1.0);
    metallic = clamp(metallic,0.0,1.0);
    float alphaRoughness = perceptualRoughness * perceptualRoughness;
    vec4 baseColor = albedo;
    vec3 f0 = vec3(0.04);
    vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
    diffuseColor *= 1.0 - metallic;
    vec3 specularColor = mix(f0,baseColor.rgb,metallic);
    float reflectance = max(max(specularColor.r,specularColor.g),specularColor.b);
    float reflectance90 = clamp(reflectance*25.0,0.0,1.0);
    vec3 specularEnviromentR0=specularColor.rgb;
    vec3 specularEnviromentR90 = vec3(1.0,1.0,1.0) * reflectance90;
    vec3 n = normalize(normal);
    vec3 v = normalize(cameraPos - worldPos);
    vec3 reflection = -normalize(reflect(v,n));

