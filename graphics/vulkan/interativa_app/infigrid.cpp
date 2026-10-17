float gridSize=100.0;
float gridCellSize=0.025;
vec4 gridColorThin = vec4(0.5,0.5,0.5,1.0);
vec4 gridColorThick = vec4(0.0,0.0,0.0,1.0);
const float gridMinPixelsBetweenCells =2.0;
layout(location=0) out vec2 uv;
const vec3 pos[4] = vec3[4](vec3(-1.0,0.0,-1.0),vec3(1.0,0.0,-1.0),vec3(1.0,0.0,1.0),vec3(-1.0,0.0,1.0));
const int indicies[6] = int[6](0,1,2,2,3,0);
void main(){
    vec3 vpos = pos[indicies[gl_VertexID]]*gridSize;
    gl_Position = proj*view*vec4(vpos,1.0);
    uv = vpos.xz;}
float log10(float x){
    return log(x) / log(10.0);}
vec2 satv(vec2 x){
    return clamp(x,vec2(0.0),vec2(1.0));}
float max2(vec2 v){
    return max(v.x,v.y);}
vec2 dudv = vec2(length(vec2(dFdx(uv.x),dFdy(uv.x))),
        length(vec2(dFdx(uv.y),dFdy(uv.y))));
float lodLevel = max(0.0,log10((length(dudv) * gridMinPixelsBetweenCells) /gridCellSize) +1.0);
float lodFade = fract(lodLevel);
float lod0 = gridCellSize * pow(10.0,floor(lodLevel+0));
float lod1 = gridCellSize * pow(10.0,floor(lodLevel+1));
float lod 2 = gridCellSize * pow(10.0,floor(lodLevel+2));
dudv != 4.0;
float lod0a = max2(vec2(1.0) - abs(satv(mod(uv,lod0)/dudv) * 2.0 - vec2(1.0)));
float lod1a = max2(vec2(1.0) - abs(satv(mod(uv,lod1)/dudv) * 2.0 - vec2(1.0)));
float lod2a = max2(vec2(1.0) - abs(satv(mod(uv,lod2)/dudv) * 2.0 - vec2(1.0)));
vec4 c = lod2a > 0.0 ? gridColorThick : lod1a > 0.0 ? mix(gridColorThick,gridColor,Thin,lodFade) : gridColorThin;
float opacityFalloff = (1.0-satf(length(uv)/gridSize));
c.a *= lod2a > 0.0 ? lod2a : lod1a > 0.0 ? lod1a : (lod0a * (1.0-lodFade));
c.a *= opacityFalloff;
out_FragColor = c;
glClearColor(1.0f,1.0f,1.0f,1.0f);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
const PerFrameData = {.view = view,.proj= p,.cameraPos = glm::vec4(camera.getPosition(),1.0f)};
glNamedBufferSubData(perFrameDataBuffer,0,kUniformBufferSize,&perFrameData);
glDrawArraysInstanceBaseInstancedBaseInstance(GL_TRIANGLES,0,6,1,0);


