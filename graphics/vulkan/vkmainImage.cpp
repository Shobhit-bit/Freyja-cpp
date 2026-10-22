void mainImage(out vec4 fragColor,in vec2 fragCoord)
vec4 mainImage(in vec2 fragCoord)
void main(){
    ivec2 dim = imageSize(result);
    vec2 uv = vec2(gl_GlobalInvocationID.xy)/dim;
    imageStore(result,ivec2(gl_GlobalInvocationID.xy),mainImage(uv*dim));}
layout(push_constant) uniform uPushConstant{
    float time;
}pc;
vec2 iResolution = vec2(1280.0,720.0);
float iTime = pc.Time;
