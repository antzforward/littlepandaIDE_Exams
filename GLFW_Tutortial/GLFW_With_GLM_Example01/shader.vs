#version 330 core
layout (location = 0) in vec3 position;//in 代表输入向量， location，与下面的顶点属性描述有关。
layout (location = 1) in vec2 uv;

uniform mat4 mvp;  // GLM计算的矩阵
//这些是shader toy的用法。
uniform vec2 iResolution;
uniform float iTime;
uniform float iTimeDelta;
uniform int iFrame;
uniform vec4 iMouse;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
out vec2 outUV;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
    outUV = uv;
}
