#version 330 core

// Per Vertex
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_Transform;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform float u_NumberOfRows; 
uniform vec2 u_Offset; 

out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_Position;

void main()
{
    gl_Position = u_Projection * u_View * u_Transform * vec4(a_Position, 1.0);
    v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
    v_Position = vec3(u_Transform * vec4(a_Position, 1.0));
    v_TexCoord = (a_TexCoord / u_NumberOfRows) + u_Offset;
}