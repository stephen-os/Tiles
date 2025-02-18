#version 330 core
in vec2 v_ScreenPos;
out vec4 FragColor;

uniform mat4 u_ViewProjection;
uniform float u_AspectRatio;
uniform float u_GridSpacing;
uniform vec3 u_GridColor1;
uniform vec3 u_GridColor2;
uniform vec2 u_GridSize;

uniform float u_BlackLineThickness = 0.2; // Adjust thickness of black grid lines
uniform float u_RedLineThickness = 0.3;   // Adjust thickness of red anchor lines

void main()
{
    // Convert screen position to world space
    vec2 screenPos = v_ScreenPos * vec2(u_AspectRatio, 1.0);
    vec4 worldPos = inverse(u_ViewProjection) * vec4(screenPos, 0.0, 1.0);
    worldPos /= worldPos.w;

    // Calculate grid coordinates
    vec2 gridCoord = worldPos.xy / u_GridSpacing;
    ivec2 cell = ivec2(floor(gridCoord));
    vec2 cellPos = fract(gridCoord);

    // Grid line conditions
    bool isOnVerticalLine = (cell.x % 4 == 0) && (cellPos.x < u_BlackLineThickness);
    bool isOnHorizontalLine = (cell.y % 4 == 0) && (cellPos.y < u_BlackLineThickness);

    // Anchor and corner red lines
    bool isAnchorVertical   = (cell.x == 0) && (cellPos.x < u_RedLineThickness);
    bool isAnchorHorizontal = (cell.y == 0) && (cellPos.y < u_RedLineThickness);
    bool isCornerVertical   = (cell.x == int(u_GridSize.x)) && (cellPos.x < u_RedLineThickness);
    bool isCornerHorizontal = (cell.y == int(u_GridSize.y)) && (cellPos.y < u_RedLineThickness);

    // Assign colors
    vec3 gridColor = ((cell.x + cell.y) % 2 == 0) ? u_GridColor1 : u_GridColor2;
    vec3 finalColor = gridColor;

    if (isOnVerticalLine || isOnHorizontalLine)
        finalColor = vec3(0.2); // Black grid lines

    if (isAnchorVertical || isAnchorHorizontal || isCornerVertical || isCornerHorizontal)
        finalColor = vec3(1.0, 0.0, 0.0); // Red anchor lines

    FragColor = vec4(finalColor, 1.0);
}
