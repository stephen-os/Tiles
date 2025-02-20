#version 330 core
in vec2 v_ScreenPos;
out vec4 FragColor;

uniform mat4 u_ViewProjection;
uniform float u_AspectRatio;
uniform vec2 u_GridSize;

const float GRID_SPACING = 0.01;
const float RED_LINE_THICKNESS = 0.3;
const float BLACK_LINE_THICKNESS = 0.015;
const vec3 GRID_COLOR_1 = vec3(0.47, 0.47, 0.47);     // Inner light gray
const vec3 GRID_COLOR_2 = vec3(0.31, 0.31, 0.31);     // Inner dark gray
const vec3 GRID_COLOR_3 = vec3(0.27, 0.27, 0.27);     // Outer light gray
const vec3 GRID_COLOR_4 = vec3(0.15, 0.15, 0.15);     // Outer dark gray

void main()
{
    // Convert screen position to world space
    vec2 screenPos = v_ScreenPos * vec2(u_AspectRatio, 1.0);
    vec4 worldPos = inverse(u_ViewProjection) * vec4(screenPos, 0.0, 1.0);
    worldPos /= worldPos.w;

    // Calculate grid coordinates
    vec2 gridCoord = worldPos.xy / GRID_SPACING;
    ivec2 cell = ivec2(floor(gridCoord));
    vec2 cellPos = fract(gridCoord);

    // Calculate position within 4x4 group
    ivec2 groupCell = cell / 4;
    vec2 groupLocalPos = fract(gridCoord / 4.0);

    // Check if we're outside the boundary
    bool isOutsideBoundary = 
        gridCoord.x < 0.0 || 
        gridCoord.x > float(u_GridSize.x) ||
        gridCoord.y < 0.0 || 
        gridCoord.y > float(u_GridSize.y);

    // Grid line conditions for 4x4 groups
    bool isOnVerticalLine = (
        (groupLocalPos.x > (1.0 - BLACK_LINE_THICKNESS)) ||
        (groupLocalPos.x < BLACK_LINE_THICKNESS)
    );
    bool isOnHorizontalLine = (
        (groupLocalPos.y > (1.0 - BLACK_LINE_THICKNESS)) ||
        (groupLocalPos.y < BLACK_LINE_THICKNESS)
    );

    // Modified red boundary lines
    bool isAnchorVertical = (gridCoord.x < 0.0 && gridCoord.x > -RED_LINE_THICKNESS);
    bool isCornerVertical = (gridCoord.x > float(u_GridSize.x) && gridCoord.x < float(u_GridSize.x) + RED_LINE_THICKNESS);
    
    bool isAnchorHorizontal = (gridCoord.y < 0.0 && gridCoord.y > -RED_LINE_THICKNESS);
    bool isCornerHorizontal = (gridCoord.y > float(u_GridSize.y) && gridCoord.y < float(u_GridSize.y) + RED_LINE_THICKNESS);

    // Assign colors based on position
    vec3 gridColor;
    if (isOutsideBoundary) {
        // Use darker colors outside boundary
        gridColor = ((cell.x + cell.y) % 2 == 0) ? GRID_COLOR_3 : GRID_COLOR_4;
    } else {
        // Use normal colors inside boundary
        gridColor = ((cell.x + cell.y) % 2 == 0) ? GRID_COLOR_1 : GRID_COLOR_2;
    }

    vec3 finalColor = gridColor;
    
    if (isOnVerticalLine || isOnHorizontalLine)
        finalColor = vec3(0.2); // Black grid lines
        
    if (isAnchorVertical || isAnchorHorizontal || isCornerVertical || isCornerHorizontal)
        finalColor = vec3(1.0, 0.0, 0.0); // Red anchor lines

    FragColor = vec4(finalColor, 1.0);
}