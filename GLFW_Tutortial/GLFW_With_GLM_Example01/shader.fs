#version 330 core
in vec2 outUV;

out vec4 color;
uniform vec2 iResolution;
uniform float iTime;
uniform float iTimeDelta;
uniform int iFrame;
uniform vec4 iMouse;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3; 
void main()
{
    color = vec4(outUV,0.0, 1.0);
}
