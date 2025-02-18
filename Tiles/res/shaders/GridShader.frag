#version 330 core
in vec2 v_ScreenPos;
out vec4 FragColor;

uniform mat4 u_ViewProjection;
uniform float u_AspectRatio;
uniform float u_GridSpacing = 1.0;
uniform vec3 u_GridColor1 = vec3(0.47, 0.47, 0.47);
uniform vec3 u_GridColor2 = vec3(0.31, 0.31, 0.31);
uniform vec2 u_GridSize;

void main()
{
    // Convert screen position to world space
    vec2 screenPos = v_ScreenPos * vec2(u_AspectRatio, 1.0);
    
    // Get the inverse view-projection matrix to transform from screen to world space
    mat4 invViewProj = inverse(u_ViewProjection);
    
    // Transform to world space
    vec4 worldPos = invViewProj * vec4(screenPos, 0.0, 1.0);
    worldPos /= worldPos.w;
    
    // Calculate grid coordinates in world space
    vec2 gridCoord = worldPos.xy / u_GridSpacing;
    
    // Get the cell coordinates
    ivec2 cell = ivec2(floor(gridCoord));
    
    // Calculate position within the current cell
    vec2 cellPos = fract(gridCoord);
    
    // Determine if we're on a major grid line (every 4 tiles)
    bool isOnVerticalLine = (cell.x % 4 == 0) && (cellPos.x < 0.1);
    bool isOnHorizontalLine = (cell.y % 4 == 0) && (cellPos.y < 0.1);
    

    // Red Line
    bool isLightTile = (cell.x + cell.y) % 2 == 0;
    vec3 color = isLightTile ? u_GridColor1 : u_GridColor2;

    // Increase the threshold to make the red lines thicker
    float lineThickness = 0.2; // Adjust this value for thicker/thinner lines

    bool isAnchorVertical   = (cell.x == 0) && (cellPos.x < lineThickness);
    bool isAnchorHorizontal = (cell.y == 0) && (cellPos.y < lineThickness);

    // Red lines at the second corner (u_GridSize.x, u_GridSize.y)
    bool isCornerVertical   = (cell.x == int(u_GridSize.x)) && (cellPos.x < lineThickness);
    bool isCornerHorizontal = (cell.y == int(u_GridSize.y)) && (cellPos.y < lineThickness);

    if (isOnVerticalLine || isOnHorizontalLine) 
    {
        // Use a darker color for the grid lines
        FragColor = vec4(0.2, 0.2, 0.2, 1.0);
    } 
    else 
    {
        // Regular checkerboard pattern
        bool isLightTile = (cell.x + cell.y) % 2 == 0;
        vec3 color = isLightTile ? u_GridColor1 : u_GridColor2;
        FragColor = vec4(color, 1.0);
    }

    if (isAnchorVertical || isAnchorHorizontal || isCornerVertical || isCornerHorizontal) 
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color for the special grid lines
    }
}