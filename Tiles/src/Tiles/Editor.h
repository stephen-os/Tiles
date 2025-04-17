#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Lumina/Core/Layer.h"

#include "Lumina/Utils/Timer.h"
#include "Lumina/Utils/FileReader.h"
#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Core/Aliases.h"

// Client
#include "Panels/HeaderPanel.h"
#include "Panels/TextureSelectionPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/LayerSelectionPanel.h"
#include "Panels/ToolSelectionPanel.h"

#include "Core/TextureSelection.h"
#include "Core/ToolSelection.h"
#include "Core/TileRenderer.h"
#include "Core/Layers.h"
#include "Core/Color.h"

#include "Commands/CommandHistory.h"

namespace Tiles
{
    class Editor : public Lumina::Layer
    {
    public:
        virtual void OnUpdate(float timestep) override
        {
            float elapsedTime = m_FrameTimer.Elapsed();
            m_FPS = 1.0f / elapsedTime;
            m_FrameTimer.Reset();
        }

        virtual void OnUIRender() override
        {
            ImGui::Begin("FPS Counter");
            ImGui::Text("FPS: %.1f", m_FPS);
            ImGui::End();

            ImGui::Begin("Statistic");
            Lumina::Renderer::Statistics stats = Lumina::Renderer::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quad Count: %d", stats.QuadCount);
            ImGui::Text("Textures Used: %d", stats.TexturesUsed);
            ImGui::Text("Shaders Used: %d", stats.ShadersUsed);
            Lumina::Renderer::ResetStats();
            ImGui::End();

            m_HeaderPanel.OnUIRender();
            m_TextureSelectionPanel.OnUIRender();
            m_ViewportPanel.OnUIRender();
            m_ToolSelectionPanel.Render();
            m_LayerSelectionPanel.OnUIRender();
        }

        virtual void OnAttach() override
        {
            TileRenderer::Init();

            Theme::ApplyTheme();

            // References
            Shared<Layers> layers = MakeShared<Layers>();
            Shared<Lumina::TextureAtlas> atlas = MakeShared<Lumina::TextureAtlas>(16, 16);
            Shared<ToolSelection> toolSelection = MakeShared<ToolSelection>();
            Shared<TextureSelection> textureSelection = MakeShared<TextureSelection>();
            Shared<CommandHistory> commandHistory = MakeShared<CommandHistory>(layers);

            layers->Resize(16, 16);

            // Header
            m_HeaderPanel.SetLayers(layers);
            m_HeaderPanel.SetTextureAtlas(atlas);
            m_HeaderPanel.SetTextureSelection(textureSelection);
            m_HeaderPanel.SetCommandHistory(commandHistory);

            // Viewport
            m_ViewportPanel.SetLayers(layers);
            m_ViewportPanel.SetTextureAtlas(atlas);
            m_ViewportPanel.SetToolSelection(toolSelection);
            m_ViewportPanel.SetTextureSelection(textureSelection);
            m_ViewportPanel.SetCommandHistory(commandHistory);

            // Texture Selection
            m_TextureSelectionPanel.SetTextureAtlas(atlas);
            m_TextureSelectionPanel.SetTextureSelection(textureSelection);
            m_TextureSelectionPanel.SetToolSelection(toolSelection);

            // Layer Selection
            m_LayerSelectionPanel.SetLayers(layers);
            m_LayerSelectionPanel.SetCommandHistory(commandHistory);

            // Tool Selection
            m_ToolSelectionPanel.SetToolSelection(toolSelection);
            m_ToolSelectionPanel.SetTextureSelection(textureSelection);
        }

        virtual void OnDetach() override
        {
			TileRenderer::Shutdown();
        }

    private:
        // Panels
        HeaderPanel m_HeaderPanel;
        TextureSelectionPanel m_TextureSelectionPanel;
        ViewportPanel m_ViewportPanel;
        LayerSelectionPanel m_LayerSelectionPanel;
        ToolSelectionPanel m_ToolSelectionPanel;

        // Util
        Lumina::Timer m_FrameTimer;
        float m_FPS = 0;
    };
}