#version 460 core

// Fullscreen quad in clip space. Each fragment reconstructs its world-space
// position by inverting the camera's view-projection, so the grid is a function
// of world coordinates and is therefore infinite.

layout(location = 0) in vec2 a_NDC;

out vec2 v_WorldPos;

uniform mat4 u_InvViewProjection;

void main()
{
    vec4 world = u_InvViewProjection * vec4(a_NDC, 0.0, 1.0);
    v_WorldPos = world.xy / world.w;

    gl_Position = vec4(a_NDC, 0.0, 1.0);
}
