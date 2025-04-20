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
#include "Panels/TintSelectionPanel.h"

#include "Core/TileAttributes.h"
#include "Core/TileRenderer.h"
#include "Core/Layers.h"
#include "Core/Color.h"
#include "Core/Selection.h"

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

#ifdef LUMINA_DEBUG
            ImGui::Begin("FPS Counter");
            ImGui::Text("FPS: %.1f", m_FPS);
            ImGui::End();

            ImGui::Begin("Debug Data");
            Lumina::Renderer::Statistics stats = Lumina::Renderer::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quad Count: %d", stats.QuadCount);
            ImGui::Text("Textures Used: %d", stats.TexturesUsed);
            ImGui::Text("Shaders Used: %d", stats.ShadersUsed);
            ImGui::Text("Mode Selection: %d", Selection::GetCurrentMode());
            Lumina::Renderer::ResetStats();
            ImGui::End();
#endif
            m_HeaderPanel.OnUIRender();
            m_TextureSelectionPanel.OnUIRender();
            m_ViewportPanel.OnUIRender();
            m_ToolSelectionPanel.Render();
            m_LayerSelectionPanel.OnUIRender();
            m_TintSelectionPanel.OnUIRender(); 
        }

        virtual void OnAttach() override
        {
            TileRenderer::Init();

            Theme::ApplyTheme();

            // References
            Shared<Layers> layers = MakeShared<Layers>();
            Shared<Lumina::TextureAtlas> atlas = MakeShared<Lumina::TextureAtlas>(16, 16);
            Shared<TileAttributes> tileAttributes = MakeShared<TileAttributes>();
            Shared<CommandHistory> commandHistory = MakeShared<CommandHistory>(layers);

            layers->Resize(16, 16);

            // Header
            m_HeaderPanel.SetLayers(layers);
            m_HeaderPanel.SetTextureAtlas(atlas);
            m_HeaderPanel.SetCommandHistory(commandHistory);

            // Viewport
            m_ViewportPanel.SetLayers(layers);
            m_ViewportPanel.SetTextureAtlas(atlas);
            m_ViewportPanel.SetTileAttributes(tileAttributes);
            m_ViewportPanel.SetCommandHistory(commandHistory);

            // Texture Selection
            m_TextureSelectionPanel.SetTextureAtlas(atlas);
            m_TextureSelectionPanel.SetTileAttributes(tileAttributes);

            // Layer Selection
            m_LayerSelectionPanel.SetLayers(layers);
            m_LayerSelectionPanel.SetCommandHistory(commandHistory);

            // Tool Selection
            m_ToolSelectionPanel.SetTileAttributes(tileAttributes);

			// Tint Selection
			m_TintSelectionPanel.SetTileAttributes(tileAttributes);
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
		TintSelectionPanel m_TintSelectionPanel;

        // Util
        Lumina::Timer m_FrameTimer;
        float m_FPS = 0;
    };
}