#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Lumina/Layer.h"

#include "Lumina/Utils/Timer.h"
#include "Lumina/Utils/FileReader.h"
#include "Lumina/Renderer/Renderer.h"

// Client
#include "Panels/HeaderPanel.h"
#include "Panels/TextureSelectionPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/LayerSelectionPanel.h"
#include "Panels/ToolSelectionPanel.h"

#include "Core/TextureSelection.h"
#include "Core/ToolSelection.h"
#include "Core/Layers.h"
#include "Core/Atlas.h"
#include "Core/Color.h"
#include "Core/Base.h"

#include "Commands/CommandHistory.h"

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

        m_HeaderPanel.OnUIRender();
        m_TextureSelectionPanel.OnUIRender();
        m_ViewportPanel.OnUIRender();
        m_ToolSelectionPanel.Render();
        m_LayerSelectionPanel.OnUIRender();
    }

    virtual void OnAttach() override
    {
        Lumina::Renderer::Init();

        Tiles::Theme::ApplyTheme();

        // References
        Tiles::Shared<Tiles::Layers> layers = Tiles::MakeShared<Tiles::Layers>();
        Tiles::Shared<Tiles::Atlas> atlas = Tiles::MakeShared<Tiles::Atlas>();
        Tiles::Shared<Tiles::ToolSelection> toolSelection = Tiles::MakeShared<Tiles::ToolSelection>();
        Tiles::Shared<Tiles::TextureSelection> textureSelection = Tiles::MakeShared<Tiles::TextureSelection>();
        Tiles::Shared<Tiles::CommandHistory> commandHistory = Tiles::MakeShared<Tiles::CommandHistory>(layers);

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
        Lumina::Renderer::Shutdown();
    }

private:
    // Panels
    Tiles::HeaderPanel m_HeaderPanel;
    Tiles::TextureSelectionPanel m_TextureSelectionPanel;
    Tiles::ViewportPanel m_ViewportPanel;
    Tiles::LayerSelectionPanel m_LayerSelectionPanel;
    Tiles::ToolSelectionPanel m_ToolSelectionPanel;

    // Util
    Lumina::Timer m_FrameTimer;
    float m_FPS = 0;
};