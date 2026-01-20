#version 450 core
// 使用450 core版本，这是SPIR-V的推荐版本

// 使用更明确的变量名
layout(location = 0) in vec2 a_position;    // 顶点位置
layout(location = 1) in vec2 a_texCoord;    // 纹理坐标
// Shadertoy风格的uniforms - 所有非不透明uniform都需要指定location
layout(location = 0) uniform vec2 iResolution;
layout(location = 1) uniform float iTime;
layout(location = 2) uniform float iTimeDelta;
layout(location = 3) uniform int iFrame;
layout(location = 4) uniform vec4 iMouse;
layout(location = 5) uniform mat4 mvp;
// 输出到片段着色器
layout(location = 0) out vec2 v_texCoord;

void main() {
    gl_Position = mvp*vec4(a_position, 0.0, 1.0);
    v_texCoord = a_texCoord;
}