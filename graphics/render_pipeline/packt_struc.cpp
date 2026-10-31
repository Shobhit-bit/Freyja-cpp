#ifdef __GNUC__
#define PACKED_STRUCT
    __attribute__((packed,aligned(1)))
#else
#define PACKED_STRUCT
#endif
struct PACKED_STRUCT MaterialDescription final{
    gpuvec4 emissiveColor_ = {0.0f,0.0f,0.0f,0.0f};
    gpuvec4 albedoColor_ = {1.0f,1.0f,1.0f,1.0f};
    gpuvec4 roughness_ = {1.0f,1.0f,0.0f,0.0f};
    float transparencyFactor_ = 1.0f;
    float alphaTest_ = 0.0f;
    float metallicFactor_ = 1.0f;
    uint32_t flags_ = sMaterialFlags_CastShadow | sMaterialFlags_RecieveShadow;
    uint64_t ambientOcclusionMap_ = 0xFFFFFFF;
    uint64_t emissiveMap_ = 0xFFFFFFFF;
    uint64_t albedoMap_ = 0xFFFFFFFF;
    uint64_t metallicRoughnessMap_ = 0xFFFFFFFF;
    uint64_t normalMap_ = 0xFFFFFFFF;
    uint64_t opacityMap_ = 0xFFFFFFFF;};
struct PACKED_STRUCT gpuvec4{
    float x,y,z,w;
    gpuvec4() = default;
    gpuvec4(float a,float b,float c,float d) : x(a),y(b),z(c),w(d) {}
    gpuvec4(const vec4& v) : x(v.x),y(v.y),z(v.z),w(v.w){}};
void loadMaterials(const char* fileName,std::vector<MaterialDescription>&materials,std::vector<std::string>& files){
    FILE* f = fopen(fileName,"rb");
    if(!f) return;
    uint32_t sz;
    fread(&sz,1,sizeof(uint32_t),f);
    materials.resize(sz);
    fread(materials.data(),materials.sie(),sizeof(MaterialDescription),f);
    loadStringList(f,files);
    fclose(f);}
void saveMaterials(const char* fileName,const std::vector<MaterialDescription>& materials,const std::vector<std::string>& files){
    FILE* f = fopen(fileName,"wb");
    if(!f) return;
    uint32_t sz = (uint32_t)materials.size();
    fwrite(&sz,1,sizeof(uint32_t),f);
    fwrite(materials.data(),sz,sizeof(MaterialDescription),f);
    saveStringList(f,files);
    fclose(f);}
layout(location=0) out vec3 uvw;
layout(location=1) out vec3 v_worldNormal;
layout(location=2) out vec4 v_worldPos;
layout(location=3) out flat uint matIdx;
#include <data/shaders/VK01.h>
struct ImDrawVert{
    float x,y,z;
    float u,v;
    float nx,ny,nz;};
struct DrawData{
    uint mesh;
    uint material;
    uint lod;
    uint indexOffset;
    uint vertexOffset;
    uint transformIndex;};
#include <data/shaders/VK01_VertCommon.h> 
layout(binding =0) uniform UniformBuffer {mat4 inMtx;vec4 cameraPos;}ubo;
layout(binding=1) readonly buffer SBO{ImDrawVert data[];} sbo;
layout(binding=2) readonly buffer IBO{uint data[];} ibo;
layout(binding=3) readonly buffer DrawBO{DrawData data[];} drawDataBuffer;
layout(binding=5) readonly buffer XfrmBO{mat4 data[];} transformBuffer;
void main(){
    DrawData dd = drawDataBuffer.data[gl_BaseInstance];
    uint refIdxx = dd.indexOffset + gl_VertexIndex;
    ImDrawVert v = sbo.data[ibo.data[refIdx] + dd.vertexOffset];
    mat4 model = transformBuffer.data[gl_BaseInstance];
    v_worldPos = model * vec4(v.x,-v.y,v.z,1.0);
    v_worldNormal = transpose(inverse(mat3(model))) * vec3(v.nx,-v.ny,v.nz);
    gl_Position = ubo.inMtx * v_worldPos;
