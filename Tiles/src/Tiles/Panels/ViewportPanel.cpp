#include "ViewportPanel.h"

#include "imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>

namespace Tiles
{
    ViewportPanel::ViewportPanel()
    {
        // Define a fullscreen quad (NDC -1 to 1)
        float vertices[] = {
            // Positions    
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f
        };

        uint32_t indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Create Vertex Array
        m_Grid = Lumina::CreateRef<Lumina::VertexArray>();

        // Create Vertex Buffer
        auto vertexBuffer = Lumina::CreateRef<Lumina::VertexBuffer>(vertices, sizeof(vertices));

        // Define the layout of the vertex data
        vertexBuffer->SetLayout({
            { Lumina::BufferDataType::Float3, "a_Position" }
            });

        // Add the vertex buffer to the vertex array
        m_Grid->AddVertexBuffer(vertexBuffer);

        // Create Index Buffer
        auto indexBuffer = Lumina::CreateRef<Lumina::IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
        m_Grid->SetIndexBuffer(indexBuffer);

        // Create Infinite Grid Shader
        const std::string vertexSrc = R"(
            #version 330 core
            layout(location = 0) in vec3 a_Position;

            out vec2 v_ScreenPos;

            void main()
            {
                gl_Position = vec4(a_Position, 1.0);
                // Convert from [-1,1] range to [0,1] range for screen position
                v_ScreenPos = (a_Position.xy + 1.0) * 0.5;
            }
        )";

        const std::string fragmentSrc = R"(
            #version 330 core
            in vec2 v_ScreenPos;
            out vec4 FragColor;

            uniform mat4 u_ViewProjection;
            uniform float u_AspectRatio;
            uniform float u_GridSpacing = 1.0;
            uniform vec3 u_GridColor1 = vec3(0.47, 0.47, 0.47);
            uniform vec3 u_GridColor2 = vec3(0.31, 0.31, 0.31);

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
    
                if (isOnVerticalLine || isOnHorizontalLine) {
                    // Use a darker color for the grid lines
                    FragColor = vec4(0.2, 0.2, 0.2, 1.0);
                } else {
                    // Regular checkerboard pattern
                    bool isLightTile = (cell.x + cell.y) % 2 == 0;
                    vec3 color = isLightTile ? u_GridColor1 : u_GridColor2;
                    FragColor = vec4(color, 1.0);
                }
            }
        )";

        m_GridShader = Lumina::CreateRef<Lumina::ShaderProgram>(vertexSrc, fragmentSrc);

        m_Renderer.Init();
    }

    void ViewportPanel::Render()
    {
        ImGui::Begin("GL Viewport");
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        m_Renderer.Begin();
        m_Renderer.OnWindowResize(viewportSize.x, viewportSize.y);

        float aspectRatio = viewportSize.x / viewportSize.y;

        // Keep a fixed world-space size regardless of screen size
        float orthoSize = 10.0f;
        float left = -orthoSize * aspectRatio;
        float right = orthoSize * aspectRatio;
        float bottom = -orthoSize;
        float top = orthoSize;
        float znear = -1.0f;
        float zfar = 1.0f;

        m_Camera.SetProjectionMatrix(left, right, bottom, top, znear, zfar);

        m_Renderer.Clear();
        m_Renderer.ClearColor(0.1f, 0.1f, 0.1f);
        m_Renderer.Enable(Lumina::RenderState::DepthTest);
        m_Renderer.Enable(Lumina::RenderState::CullFace);

        m_GridShader->Bind();

        // Update uniforms
        glm::mat4 viewProjection = m_Camera.GetViewMatrix();
        m_GridShader->SetUniformMatrix4fv("u_ViewProjection", viewProjection);
        m_GridShader->SetUniform1f("u_AspectRatio", aspectRatio);
        m_GridShader->SetUniform1f("u_GridSpacing", 0.01f);
        m_GridShader->SetUniform3fv("u_GridColor1", { 0.47f, 0.47f, 0.47f });
        m_GridShader->SetUniform3fv("u_GridColor2", { 0.31f, 0.31f, 0.31f });

        m_Renderer.Draw(m_Grid);

        ImGui::Image((void*)(intptr_t)m_Renderer.GetID(), viewportSize);

        m_Renderer.End();
        ImGui::End();
    }
}
