// 使用 cbuffer 代替 uniform
cbuffer Constants : register(b0)
{
    float2 gl_HalfPixel;
};

struct VSInput
{
    float2 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float2 outUV : TEXCOORD0;
    float4 position : SV_POSITION;  // Vulkan 中使用 SV_POSITION
};

VSOutput main(VSInput input)
{
    VSOutput output;
    
    // 直接计算位置
    float4 pos = float4(input.pos, 0.0f, 1.0f);
    
    // 应用半像素偏移
    pos.x = pos.x - gl_HalfPixel.x * pos.w;
    pos.y = pos.y + gl_HalfPixel.y * pos.w;
    
    output.position = pos;
    output.outUV = input.uv;
    
    return output;
}