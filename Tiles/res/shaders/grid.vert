#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 worldPos;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    worldPos = aPos;
    gl_Position = u_Projection * u_View * vec4(aPos, 1.0);
}