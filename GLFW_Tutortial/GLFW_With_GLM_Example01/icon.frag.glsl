#version 450 core
// Shadertoy风格的uniforms - 所有非不透明uniform都需要指定location
layout(location = 0) uniform vec2 iResolution;
layout(location = 1) uniform float iTime;
layout(location = 2) uniform float iTimeDelta;
layout(location = 3) uniform int iFrame;
layout(location = 4) uniform vec4 iMouse;

// 采样器是"不透明"类型，不需要location
layout(binding = 0) uniform sampler2D iChannel0;
layout(binding = 1) uniform sampler2D iChannel1;
layout(binding = 2) uniform sampler2D iChannel2;
layout(binding = 3) uniform sampler2D iChannel3;

// 输出颜色
layout(location = 0) out vec4 outputPS0;

vec3 palette(float t)
{
    vec3 a = vec3(0.5);
    vec3 b = vec3(0.5);
    vec3 c = vec3(1.0);
    vec3 d = vec3(0.263000011444091796875, 0.41600000858306884765625, 0.556999981403350830078125);
    return a + (b * cos(((c * t) + d) * 6.28318023681640625));
}

void mainImage(inout vec4 fragColor, vec2 fragCoord)
{
    vec2 uv = ((fragCoord * 2.0) - iResolution) / vec2(iResolution.y);
    vec2 uv0 = uv;
    vec3 finalColor = vec3(0.0);
    for (float i = 0.0; i < 4.0; i += 1.0)
    {
        uv = fract(uv * 1.5) - vec2(0.5);
        float d = length(uv) * exp(-length(uv0));
        float param = (length(uv0) + (i * 0.4000000059604644775390625)) + (iTime * 0.4000000059604644775390625);
        vec3 col = palette(param);
        d = sin((d * 8.0) + iTime) / 8.0;
        d = abs(d);
        d = pow(0.00999999977648258209228515625 / d, 1.2000000476837158203125);
        finalColor += (col * d);
    }
    fragColor = vec4(finalColor, 1.0);
}

void main()
{
    vec2 param_1 = gl_FragCoord.xy;
    vec4 param;
    mainImage(param, param_1);
    outputPS0 = param;
}

