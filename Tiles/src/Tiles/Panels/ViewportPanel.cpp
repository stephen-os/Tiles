#include "ViewportPanel.h"

#include "imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Lumina/Utils/FileReader.h"

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
        const std::string vertexSrc = Lumina::ReadFile("res/shaders/GridShader.vert");
        const std::string fragmentSrc = Lumina::ReadFile("res/shaders/GridShader.frag");

        m_GridShader = Lumina::CreateRef<Lumina::ShaderProgram>(vertexSrc, fragmentSrc);

        m_Renderer.Init();
    }

    void ViewportPanel::Render()
    {
        ImGui::Begin("GL Viewport");
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        // Get the screen position of the viewport
        ImVec2 viewportScreenPos = ImGui::GetCursorScreenPos();

        m_Renderer.Begin();
        m_Renderer.OnWindowResize(viewportSize.x, viewportSize.y);

        float aspectRatio = viewportSize.x / viewportSize.y;

        // Handle mouse input
        ImVec2 mousePos = ImGui::GetMousePos();
        mousePos.x -= viewportScreenPos.x;
        mousePos.y -= viewportScreenPos.y;

        // Check if mouse is within viewport bounds
        bool isMouseInViewport = (mousePos.x >= 0 && mousePos.x < viewportSize.x &&
            mousePos.y >= 0 && mousePos.y < viewportSize.y);

        if (isMouseInViewport)
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
            {
                if (!m_IsMiddleMouseDown)
                {
                    // Middle mouse just pressed
                    m_IsMiddleMouseDown = true;
                    m_LastMousePos = glm::vec2(mousePos.x, mousePos.y);
                }
                else
                {
                    // Middle mouse being held - calculate drag delta
                    glm::vec2 currentMousePos(mousePos.x, mousePos.y);
                    glm::vec2 mouseDelta = currentMousePos - m_LastMousePos;

                    // Convert mouse delta to world space movement
                    float orthoSize = 10.0f;
                    float worldSpaceScale = 2.0f * orthoSize / viewportSize.y;  // Scale factor to convert from screen to world space

                    // Apply the sensitivity multiplier
                    m_CameraPosition -= glm::vec2(-mouseDelta.x * worldSpaceScale * m_Sensitivity,
                        -mouseDelta.y * worldSpaceScale * m_Sensitivity);

                    m_LastMousePos = currentMousePos;
                }
            }
            else
            {
                m_IsMiddleMouseDown = false;
            }
        }

        // Keep a fixed world-space size regardless of screen size
        float orthoSize = 10.0f;
        float left = -orthoSize * aspectRatio;
        float right = orthoSize * aspectRatio;
        float bottom = -orthoSize;
        float top = orthoSize;
        float znear = -1.0f;
        float zfar = 1.0f;

        m_Camera.SetProjectionMatrix(left, right, bottom, top, znear, zfar);

        // Create view matrix with camera position
        glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_CameraPosition.x, m_CameraPosition.y, 0.0f));

        m_Renderer.Clear();
        m_Renderer.ClearColor(0.1f, 0.1f, 0.1f);
        m_Renderer.Enable(Lumina::RenderState::DepthTest);
        m_Renderer.Enable(Lumina::RenderState::CullFace);

        m_GridShader->Bind();

        // Update uniforms with the new view matrix
        m_GridShader->SetUniformMatrix4fv("u_ViewProjection", viewMatrix);
        m_GridShader->SetUniform1f("u_AspectRatio", aspectRatio);
        m_GridShader->SetUniform1f("u_GridSpacing", 0.01f);
        m_GridShader->SetUniform2fv("u_GridSize", { 20.0f * 4, 15.0f * 4 });
        m_GridShader->SetUniform3fv("u_GridColor1", { 0.47f, 0.47f, 0.47f });
        m_GridShader->SetUniform3fv("u_GridColor2", { 0.31f, 0.31f, 0.31f });

        m_Renderer.Draw(m_Grid);

        ImGui::Image((void*)(intptr_t)m_Renderer.GetID(), viewportSize);

        m_Renderer.End();
        ImGui::End();
    }

}
