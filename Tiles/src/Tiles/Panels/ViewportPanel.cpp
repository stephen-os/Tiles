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

        HandleMouseInput();

        m_Renderer.Begin();
        m_Renderer.OnWindowResize(viewportSize.x, viewportSize.y);

        m_Renderer.Clear();
        m_Renderer.ClearColor(0.1f, 0.1f, 0.1f);
        m_Renderer.Enable(Lumina::RenderState::DepthTest);
        m_Renderer.Enable(Lumina::RenderState::CullFace);

        m_GridShader->Bind();

        // Update uniforms with the new view matrix
        m_GridShader->SetUniformMatrix4fv("u_ViewProjection", m_Camera.GetViewMatrix());
        m_GridShader->SetUniform1f("u_AspectRatio", viewportSize.x / viewportSize.y);
        m_GridShader->SetUniform1f("u_GridSpacing", 0.01f);
        m_GridShader->SetUniform2fv("u_GridSize", { 20.0f * 4, 15.0f * 4 });
        m_GridShader->SetUniform3fv("u_GridColor1", { 0.47f, 0.47f, 0.47f });
        m_GridShader->SetUniform3fv("u_GridColor2", { 0.31f, 0.31f, 0.31f });

        m_Renderer.Draw(m_Grid);

        ImGui::Image((void*)(intptr_t)m_Renderer.GetID(), viewportSize);

        m_Renderer.End();
        ImGui::End();
    }

    void ViewportPanel::HandleMouseInput()
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        ImVec2 viewportScreenPos = ImGui::GetCursorScreenPos();

        // Handle mouse input
        ImVec2 mousePos = ImGui::GetMousePos();
        mousePos.x -= viewportScreenPos.x;
        mousePos.y -= viewportScreenPos.y;

        // Check if mouse is within viewport bounds
        if (!IsMouseInViewport(mousePos, viewportSize))
            return; 

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

                m_Camera.Drag(mouseDelta);

                m_LastMousePos = currentMousePos;
            }
        }
        else
        {
            m_IsMiddleMouseDown = false;
        }

        // Handle zoom input
        float mouseWheel = ImGui::GetIO().MouseWheel;
        if (mouseWheel != 0.0f)
        {
            m_Camera.Zoom(mouseWheel);
        }
    }

    bool ViewportPanel::IsMouseInViewport(ImVec2& mousePos, ImVec2& viewportSize)
    {
        return (mousePos.x >= 0 && mousePos.x < viewportSize.x && mousePos.y >= 0 && mousePos.y < viewportSize.y);
    }

}
