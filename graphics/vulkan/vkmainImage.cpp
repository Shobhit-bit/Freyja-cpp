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
void insertComputedImageBarrier(VkCommandBuffer commandBuffer,VkImage image){
    const VkImageMemoryBarrier barrier = { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,.dstAccessMask = VK_ACCESS_ShADER_READ_BIT,.oldLayout = VK_IMAGE_LAYOUT_GENERAL,.newLayout = VK_IMAGE_LAYOUT_GENERAL,.image = image,.subresourceRange = VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1}};
    vkCmdPipelineBarrier(commandBuffer,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_PIPELINE_STAGE_FRAGMNET_SHADER_BIT,0,0,nullptr,0,nullptr,1,&barrier);}

