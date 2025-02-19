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

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 mousePos = ImGui::GetMousePos();

        HandleMouseInput();

        m_Renderer.Begin();
        m_Renderer.OnWindowResize(viewportSize.x, viewportSize.y);

        m_Renderer.Clear();
        m_Renderer.ClearColor(0.1f, 0.1f, 0.1f);
        m_Renderer.Enable(Lumina::RenderState::DepthTest);
        m_Renderer.Enable(Lumina::RenderState::CullFace);

        m_GridShader->Bind();

        glm::vec2 gridSize = { 20, 15 };

        // Update uniforms with the new view matrix
        m_GridShader->SetUniformMatrix4fv("u_ViewProjection", m_Camera.GetViewMatrix());
        m_GridShader->SetUniform1f("u_AspectRatio", viewportSize.x / viewportSize.y);
        m_GridShader->SetUniform1f("u_GridSpacing", 0.01f);
        m_GridShader->SetUniform2fv("u_GridSize", { gridSize.x * 4.0f, gridSize.y * 4.0f });
        m_GridShader->SetUniform3fv("u_GridColor1", { 0.47f, 0.47f, 0.47f });
        m_GridShader->SetUniform3fv("u_GridColor2", { 0.31f, 0.31f, 0.31f });

        m_Renderer.Draw(m_Grid);

        ImGui::Image((void*)(intptr_t)m_Renderer.GetID(), viewportSize);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f); // Remove roundness

        // Set transparent colors for the button fill
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));        // Fully transparent button
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.0f, 0.0f)); // Transparent when hovered
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));  // Transparent when clicked

        // Set border colors and thickness
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));        // Transparent border by default
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));  // Transparent border shadow
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);                       // Border thickness

        glm::vec2 camPos = m_Camera.GetPosition();

        for (int i = 0; i < gridSize.x; ++i)
        {
            for (int j = 0; j < gridSize.y; ++j)
            {
                ImVec2 buttonSize = ImVec2(40 * m_Camera.GetZoom(), 40 * m_Camera.GetZoom());
                ImGui::SetCursorPos(ImVec2(8 + (1000 * camPos.x) + (i * buttonSize.x), (30 + 1000 * camPos.y) + (j * buttonSize.y)));

                // Create unique ID for each button
                ImGui::PushID(i + j * gridSize.x);

                // Check if button is being hovered before drawing it
                ImGui::SetNextItemAllowOverlap();  // Allow buttons to overlap if needed
                bool hovered = ImGui::IsMouseHoveringRect(
                    ImGui::GetCursorScreenPos(),
                    ImVec2(ImGui::GetCursorScreenPos().x + buttonSize.x,
                        ImGui::GetCursorScreenPos().y + buttonSize.y)
                );

                // Change border color based on hover state
                if (hovered)
                {
                    ImGui::PopStyleColor();  // Pop previous border colors
                    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 165, 0, 255));        // White border
                }

                ImGui::Button("B", buttonSize);

                // Restore border colors if we changed them
                if (hovered)
                {
                    ImGui::PopStyleColor();
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                }

                ImGui::PopID();
            }
        }

        // Restore previous styles
        ImGui::PopStyleVar(2);  // Pop FrameRounding and FrameBorderSize
        ImGui::PopStyleColor(5); // Pop all color styles we pushed

        m_Renderer.End();
        ImGui::End();

        ImGui::Begin("Hovered Info");
        if (IsMouseInViewport(mousePos, windowPos, windowSize))
        {
            ImGui::Text("Mouse Position: (%.2f, %.2f)", mousePos.x, mousePos.y);
            ImGui::Text("World Position: (%.2f, %.2f)", m_Camera.GetPosition().x, m_Camera.GetPosition().y);
        }

        ImGui::End();
    }

    void ViewportPanel::HandleMouseInput()
    {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 mousePos = ImGui::GetMousePos();

        // Check if mouse is within viewport bounds
        if (!IsMouseInViewport(mousePos, windowPos, windowSize))
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

                m_AnchorePos = m_Camera.GetPosition();

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

    bool ViewportPanel::IsMouseInViewport(const ImVec2& mousePos, const ImVec2& windowPos, const ImVec2& windowSize)
    {
        return (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
            mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y);
    }

}
