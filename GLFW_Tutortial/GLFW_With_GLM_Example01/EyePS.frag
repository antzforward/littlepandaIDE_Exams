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

float hash(float n)
{
    return fract(sin(n) * 43758.546875);
}

float _noise(vec2 x)
{
    vec2 i = floor(x);
    vec2 f = fract(x);
    f = (f * f) * (vec2(3.0) - (f * 2.0));
    float n = i.x + (i.y * 57.0);
    float param = n + 0.0;
    float param_1 = n + 1.0;
    float param_2 = n + 57.0;
    float param_3 = n + 58.0;
    return mix(mix(hash(param), hash(param_1), f.x), mix(hash(param_2), hash(param_3), f.x), f.y);
}

float fbm(inout vec2 p)
{
    float f = 0.0;
    vec2 param = p;
    f += (0.5 * _noise(param));
    p = (mat2(vec2(0.800000011920928955078125, 0.60000002384185791015625), vec2(-0.60000002384185791015625, 0.800000011920928955078125)) * p) * 2.019999980926513671875;
    vec2 param_1 = p;
    f += (0.25 * _noise(param_1));
    p = (mat2(vec2(0.80000001184185791015625, 0.60000002384185791015625), vec2(-0.60000002384185791015625, 0.800000011920928955078125)) * p) * 2.0299999713897705078125;
    vec2 param_2 = p;
    f += (0.125 * _noise(param_2));
    p = (mat2(vec2(0.800000011920928955078125, 0.60000002384185791015625), vec2(-0.60000002384185791015625, 0.800000011920928955078125)) * p) * 2.0099999904632568359375;
    vec2 param_3 = p;
    f += (0.0625 * _noise(param_3));
    p = (mat2(vec2(0.800000011920928955078125, 0.60000002384185791015625), vec2(-0.60000002384185791015625, 0.800000011920928955078125)) * p) * 2.03999996185302734375;
    vec2 param_4 = p;
    f += (0.03125 * _noise(param_4));
    return f / 0.984375;
}

float length2(vec2 p)
{
    vec2 q = ((p * p) * p) * p;
    return pow(q.x + q.y, 0.25);
}

void mainImage(out vec4 fragColor, vec2 fragCoord)
{
    vec2 p = ((fragCoord * 2.0) - iResolution) / vec2(iResolution.y);
    float r = length(p);
    float a = atan(p.y, p.x);
    r *= (1.0 + ((0.20000000298023223876953125 * clamp(1.0 - r, 0.0, 1.0)) * sin(4.0 * iTime)));
    vec3 col = vec3(0.0, 0.300000011920928955078125, 0.4000000059604644775390625);
    vec2 param = p * 5.0;
    float _222 = fbm(param);
    float f = _222;
    col = mix(col, vec3(0.20000000298023223876953125, 0.5, 0.4000000059604644775390625), vec3(f));
    col = mix(col, vec3(0.89999997615814208984375, 0.60000002384185791015625, 0.20000000298023223876953125), vec3(1.0 - smoothstep(0.20000000298023223876953125, 0.60000002384185791015625, r)));
    vec2 param_1 = vec2(15.0 * a, 10.0 * r);
    float _244 = fbm(param_1);
    f = smoothstep(0.4000000059604644775390625, 0.89999997615814208984375, _244);
    col *= (1.0 - (0.5 * f));
    vec2 param_2 = p * 20.0;
    float _256 = fbm(param_2);
    a += (0.0500000007450580596923828125 * _256);
    vec2 param_3 = vec2(20.0 * a, 6.0 * r);
    float _267 = fbm(param_3);
    f = smoothstep(0.300000011920928955078125, 1.0, _267);
    col = mix(col, vec3(1.0), vec3(f));
    col *= (1.0 - (0.25 * smoothstep(0.60000002384185791015625, 0.800000011920928955078125, r)));
    vec2 param_4 = (mat2(vec2(0.60000002384185791015625, 0.800000011920928955078125), vec2(-0.800000011920928955078125, 0.60000002384185791015625)) * (p - vec2(0.300000011920928955078125, 0.5))) * vec2(1.0, 2.0);
    f = 1.0 - smoothstep(0.0, 0.60000002384185791015625, length2(param_4));
    col += ((vec3(1.0, 0.89999997615814208984375, 0.89999997615814208984375) * f) * 0.98500001430511474609375);
    col *= vec3(0.800000011920928955078125 + (0.20000000298023223876953125 * cos(r * a)));
    f = 1.0 - smoothstep(0.20000000298023223876953125, 0.25, r);
    col = mix(col, vec3(0.0), vec3(f));
    f = smoothstep(0.790000021457672119140625, 0.819999992847442626953125, r);
    col = mix(col, vec3(1.0), vec3(f));
    vec2 q = fragCoord / iResolution;
    col *= (0.5 + (0.5 * pow((((16.0 * q.x) * q.y) * (1.0 - q.x)) * (1.0 - q.y), 0.100000001490116119384765625)));
    fragColor = vec4(col, 1.0);
}

void main()
{
    vec2 param_1 = gl_FragCoord.xy;
    vec4 param;
    mainImage(param, param_1);
    outputPS0 = param;
}