#version 450 core
layout(location = 0) in vec2 inputVS0;
layout(location = 1) in vec2 inputVS1;
// Shadertoy风格的uniforms - 所有非不透明uniform都需要指定location
layout(location = 0) uniform vec2 iResolution;
layout(location = 1) uniform float iTime;
layout(location = 2) uniform float iTimeDelta;
layout(location = 3) uniform int iFrame;
layout(location = 4) uniform vec4 iMouse;
layout(location = 5) uniform mat4 mvp;
// 输出到片段着色器
layout(location = 0) out vec2 outputVS0;
void main()
{
    gl_Position = mvp*vec4(inputVS0, 0.0, 1.0);
    outputVS0 = inputVS1;
}

