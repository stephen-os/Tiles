#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

uniform mat4 u_ViewProjection;

out vec2 v_WorldPos; 

void main()
{
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
    v_WorldPos = a_Position.xy;
}