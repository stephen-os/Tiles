#version 330 core
in vec2 v_ScreenPos;
out vec4 FragColor;
uniform mat4 u_ViewProjection;
uniform float u_AspectRatio;
uniform float u_GridSpacing;
uniform vec3 u_GridColor1;
uniform vec3 u_GridColor2;
uniform vec2 u_GridSize;
uniform float u_BlackLineThickness = 0.01; // Adjust thickness of black grid lines
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

    // Calculate position within 4x4 group
    ivec2 groupCell = cell / 4;
    vec2 groupLocalPos = fract(gridCoord / 4.0);

    // Grid line conditions for 4x4 groups
    bool isOnVerticalLine = (
        // Right edge of current group
        (groupLocalPos.x > (1.0 - u_BlackLineThickness)) ||
        // Left edge of next group
        (groupLocalPos.x < u_BlackLineThickness)
    );

    bool isOnHorizontalLine = (
        // Bottom edge of current group
        (groupLocalPos.y > (1.0 - u_BlackLineThickness)) ||
        // Top edge of next group
        (groupLocalPos.y < u_BlackLineThickness)
    );

    // Modified red boundary lines to be offset from the checkerboard edges
    bool isAnchorVertical = (gridCoord.x < 0.0 && gridCoord.x > -u_RedLineThickness);
    bool isCornerVertical = (gridCoord.x > float(u_GridSize.x) && gridCoord.x < float(u_GridSize.x) + u_RedLineThickness);
    
    bool isAnchorHorizontal = (gridCoord.y < 0.0 && gridCoord.y > -u_RedLineThickness);
    bool isCornerHorizontal = (gridCoord.y > float(u_GridSize.y) && gridCoord.y < float(u_GridSize.y) + u_RedLineThickness);

    // Assign colors
    vec3 gridColor = ((cell.x + cell.y) % 2 == 0) ? u_GridColor1 : u_GridColor2;
    vec3 finalColor = gridColor;

    if (isOnVerticalLine || isOnHorizontalLine)
        finalColor = vec3(0.2); // Black grid lines
    if (isAnchorVertical || isAnchorHorizontal || isCornerVertical || isCornerHorizontal)
        finalColor = vec3(1.0, 0.0, 0.0); // Red anchor lines

    FragColor = vec4(finalColor, 1.0);
}