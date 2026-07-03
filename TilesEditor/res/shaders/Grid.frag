#version 460 core

layout(location = 0) out vec4 o_Color;

in vec2 v_WorldPos;

uniform float u_CellSize;      // minor grid spacing, world units
uniform float u_MajorEvery;    // a major line every N minor cells
uniform vec4  u_LineColor;
uniform vec4  u_MajorLineColor;
uniform vec4  u_BackgroundColor;

// Anti-aliased grid coverage at a given cell spacing. Line thickness is measured
// in screen space via fwidth(), so lines stay ~1px crisp at any zoom. Returns 1
// on a line, 0 between lines.
float gridCoverage(vec2 worldPos, float cell)
{
    vec2 coord = worldPos / cell;
    vec2 deriv = fwidth(coord);
    vec2 dist = abs(fract(coord - 0.5) - 0.5) / max(deriv, vec2(1e-6));
    float line = min(dist.x, dist.y);
    return 1.0 - min(line, 1.0);
}

void main()
{
    float minor = gridCoverage(v_WorldPos, u_CellSize);
    float major = gridCoverage(v_WorldPos, u_CellSize * u_MajorEvery);

    vec4 color = u_BackgroundColor;
    color = mix(color, u_LineColor, minor);
    color = mix(color, u_MajorLineColor, major);

    o_Color = color;
}
