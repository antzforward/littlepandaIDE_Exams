// shader_wireframe.fs
#version 330 core
in vec3 Color;
out vec4 FragColor;

uniform vec3 wireColor;  // 边框颜色

void main()
{
    FragColor = vec4(wireColor, 1.0);  // 固定白色，默认是黑色的
}
