#version 330 core

in vec3 worldPos;

out vec4 FragColor;

uniform vec3 u_GridColor = vec3(0.8, 0.8, 0.8);         // Light gray
uniform vec3 u_BackgroundColor = vec3(0.2, 0.2, 0.2);   // Dark gray
uniform float u_GridSpacing = 1.0;                      // Space between grid lines
uniform float u_LineThickness = 0.04;                   // Thickness of grid lines
uniform vec2 u_Offset = vec2(0.0, 0.0);                 // Offset for grid translation

void main()
{
    // Apply offset to world position
    vec2 adjustedPos = worldPos.xy - u_Offset;

    // Calculate distance to nearest grid line
    float distX = abs(mod(adjustedPos.x, u_GridSpacing));
    float distY = abs(mod(adjustedPos.y, u_GridSpacing));

    // Determine if within line thickness
    float lineX = smoothstep(u_LineThickness, 0.0, distX);
    float lineY = smoothstep(u_LineThickness, 0.0, distY);

    // Combine lines and set color
    float gridIntensity = max(lineX, lineY);
    FragColor = mix(vec4(u_BackgroundColor, 1.0), vec4(u_GridColor, 1.0), gridIntensity);
}
