#version 330 core

in vec3 v_Normal;
in vec3 v_Position;
in vec2 v_TexCoord;

out vec4 FragColor;

uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform sampler2D u_Texture;

void main()
{
    // Uniform lighting (simulate ambient lighting)
    vec3 uniformLighting = lightColor;

    // Texture color
    vec4 texColor = texture(u_Texture, v_TexCoord);
    if (texColor.a < 0.1)
        discard;

    FragColor = texColor;
}