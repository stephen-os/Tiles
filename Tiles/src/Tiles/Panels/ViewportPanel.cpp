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

		m_TileAttributes.Size = { 0.02f, 0.02f };
    }

    void ViewportPanel::OnUIRender()
    {
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::SetCursorPos({ 0.0f, 0.0f });

        // 10 zoom for now until we fix that
        m_ViewportCamera.SetProjectionMatrix(1.0f * 10, m_ViewportSize.x / m_ViewportSize.y, 0.01f, 100.0f);

        Lumina::Renderer::Begin(m_ViewportCamera);

        Lumina::Renderer::SetResolution(m_ViewportSize.x, m_ViewportSize.y);

        m_BackgroundAttributes.Shader->Bind();
		// * 4.0f because 4 small squares make 1 big square
        m_BackgroundAttributes.Shader->SetUniformVec2("u_GridSize", { 4.0f * 20, 4.0f * 20 });
        m_BackgroundAttributes.Shader->Unbind();

		Lumina::Renderer::DrawQuad(m_BackgroundAttributes);

        Lumina::Renderer::End();

        ImGui::Image((void*)(intptr_t)Lumina::Renderer::GetImage(), ImVec2(m_ViewportSize.x, m_ViewportSize.y));

        ImGui::End();
    }
}