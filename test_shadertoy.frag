#version 450
#extension GL_ARB_separate_shader_objects : enable

// ShaderToy 风格的声明
uniform vec2 iResolution;      // 普通数据 uniform
uniform float iTime;           // 普通数据 uniform
uniform sampler2D iChannel0;   // 纹理采样器 uniform

out vec4 fragColor;

void main() {
    fragColor = texture(iChannel0, gl_FragCoord.xy / iResolution);
}