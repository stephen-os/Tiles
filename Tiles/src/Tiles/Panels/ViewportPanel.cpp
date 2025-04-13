#include "ViewportPanel.h"

#include "../Core/Tools.h"
#include "../Core/Color.h"
#include "../Core/Base.h"

#include "../Commands/ReplaceTileCommand.h"
#include "../Commands/ReplaceLayerCommand.h"

#include "Lumina/Utils/FileReader.h"

#include <algorithm>
#include <array>

#include <iostream>

#include <spdlog/spdlog.h>

namespace Tiles
{

    ViewportPanel::ViewportPanel()
    {
        m_ViewportCamera.SetPosition({ 0.9f, 0.9f, -1.0f });

        std::string vertexShaderSource = Lumina::ReadFile("res/shaders/Background.vert");
        std::string fragmentShaderSource = Lumina::ReadFile("res/shaders/Background.frag");
        Shared<Lumina::ShaderProgram> shader = Lumina::ShaderProgram::Create(vertexShaderSource, fragmentShaderSource);

        m_BackgroundAttributes.Shader = shader;
		m_BackgroundAttributes.Size = { 5.0f, 5.0f };

        m_TileAttributes.Position = { 0.02f, 0.02f, 0.0f };
		m_TileAttributes.Size = { 0.02f, 0.02f };
    }

    void ViewportPanel::OnUIRender()
    {
        ImGui::Begin("Zoom Controls");
		ImGui::PushItemWidth(100.0f);
		ImGui::SliderFloat("Zoom", &m_Zoom, 1.0f, 20.0f, "%.2f", 1.0f);
		ImGui::PopItemWidth();
        ImGui::End();

        ImGui::Begin("Position Controls");
		ImGui::PushItemWidth(100.0f);
		ImGui::SliderFloat("X", &m_TileAttributes.Position.x, -1.0f, 1.0f, "%.2f", 1.0f);
		ImGui::SliderFloat("Y", &m_TileAttributes.Position.y, -1.0f, 1.0f, "%.2f", 1.0f);
		ImGui::PopItemWidth();
		ImGui::End();

        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::SetCursorPos({ 0.0f, 0.0f });

        // 10 zoom for now until we fix that
        m_ViewportCamera.SetProjectionMatrix(1.0f * m_Zoom, m_ViewportSize.x / m_ViewportSize.y, 0.01f, 100.0f);
        m_ViewportCamera.HandleKeyInput(0.01f); 

        Lumina::Renderer::Begin(m_ViewportCamera);

        Lumina::Renderer::SetResolution(m_ViewportSize.x, m_ViewportSize.y);

        m_BackgroundAttributes.Shader->Bind();
		// * 4.0f because 4 small squares make 1 big square
        m_BackgroundAttributes.Shader->SetUniformVec2("u_GridSize", { 4.0f * 20, 4.0f * 20 });
        m_BackgroundAttributes.Shader->Unbind();

		Lumina::Renderer::DrawQuad(m_BackgroundAttributes);
        Lumina::Renderer::DrawQuad(m_TileAttributes);

        Lumina::Renderer::End();

        ImGui::Image((void*)(intptr_t)Lumina::Renderer::GetImage(), ImVec2(m_ViewportSize.x, m_ViewportSize.y));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));         // Fully transparent button
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));  // Transparent when hovered
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));   // Transparent when clicked
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));         // Transparent border by default
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));   // Transparent border shadow

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);                         // Remove roundness
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);                       // Border thickness

        glm::vec2 cameraPos = { m_ViewportCamera.GetPosition().x, m_ViewportCamera.GetPosition().y };
        cameraPos = cameraPos * m_ViewportSize;
        
        for (size_t y = 0; y < 1; y++)
        {
            for (size_t x = 0; x < 1; x++)
            {
				Lumina::Renderer::DrawQuad(m_TileAttributes);

                float scale = m_ViewportSize.x / 1000;
                ImVec2 buttonSize = ImVec2(m_TileSize / m_Zoom * scale, m_TileSize / m_Zoom * scale);
                ImGui::SetCursorPos(ImVec2(-cameraPos.x + (x * buttonSize.x), -cameraPos.y + (y * buttonSize.y)));

                ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                ImVec2 tileMin(cursorPos.x, cursorPos.y);
                ImVec2 tileMax(tileMin.x + buttonSize.x, tileMin.y + buttonSize.y);

                // ImGui::GetWindowDrawList()->AddRectFilled(tileMin, tileMax, IM_COL32_BLACK);
            }
        }

        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);

        ImGui::End();
    }
}