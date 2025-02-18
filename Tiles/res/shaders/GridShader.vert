#version 330 core
layout(location = 0) in vec3 a_Position;

out vec2 v_ScreenPos;

void main()
{
    gl_Position = vec4(a_Position, 1.0);
    // Convert from [-1,1] range to [0,1] range for screen position
    v_ScreenPos = (a_Position.xy + 1.0) * 0.5;
}