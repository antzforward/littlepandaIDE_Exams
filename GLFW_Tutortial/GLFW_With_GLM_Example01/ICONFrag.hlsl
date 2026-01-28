// 常量缓冲区
cbuffer ShaderParams : register(b0)
{
    float2 iResolution;      // 视口分辨率
    float iTime;            // 运行时间（秒）
    float iTimeDelta;       // 帧间隔时间
    uint iFrame;           // 帧数
    float4 iMouse;         // 鼠标坐标
    // 注意：纹理不放在常量缓冲区
};

// 纹理和采样器
Texture2D iChannel0 : register(t0);
SamplerState g_sampler0 : register(s0);

Texture2D iChannel1 : register(t1);
SamplerState g_sampler1 : register(s1);

Texture2D iChannel2 : register(t2);
SamplerState g_sampler2 : register(s2);

Texture2D iChannel3 : register(t3);
SamplerState g_sampler3 : register(s3);

// 简化版颜色调色板函数
float3 palette(float t)
{
    float3 a = float3(0.5, 0.5, 0.5);
    float3 b = float3(0.5, 0.5, 0.5);
    float3 c = float3(1.0, 1.0, 1.0);
    float3 d = float3(0.263, 0.416, 0.557);
    
    return a + b * cos(6.28318 * (c * t + d));
}

// 主图像处理函数
float4 mainImage(float2 fragCoord : SV_Position) : SV_Target
{
    // 归一化像素坐标（从0到1）
    float2 uv = fragCoord.xy / iResolution.xy;
    
    // 转换到NDC空间（-1到1）
    uv = uv * 2.0 - 1.0;
    
    // 保持宽高比
    uv.x *= iResolution.x / iResolution.y;
    
    float2 uv0 = uv;
    float3 finalColor = float3(0.0, 0.0, 0.0);
    
    for (float i = 0.0; i < 4.0; i += 1.0)
    {
        uv = frac(uv * 1.5) - 0.5;
        
        float d = length(uv) * exp(-length(uv0));
        float param = length(uv0) + (i * 0.4) + (iTime * 0.4);
        float3 col = palette(param);
        
        d = sin((d * 8.0) + iTime) / 8.0;
        d = abs(d);
        d = pow(0.01 / d, 1.2);
        finalColor += (col * d);
    }
    
    return float4(finalColor, 1.0);
}

// 主函数
float4 main(float4 position : SV_Position) : SV_Target
{
    return mainImage(position.xy);
}