#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Lumina/Layer.h"

#include "Lumina/Utils/Timer.h"
#include "Lumina/Utils/FileReader.h"
#include "Lumina/Base.h"

// Client
#include "Theme/ThemeManager.h"

#include "Panels/HeaderPanel.h"
#include "Panels/TextureSelectionPanel.h"
#include "Panels/TileViewportPanel.h"
#include "Panels/LayerSelectionPanel.h"
#include "Panels/ToolSelectionPanel.h"

#include "Core/ToolModes.h"
#include "Core/Layers.h"
#include "Core/Atlas.h"
#include "Core/State.h"

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

        m_HeaderPanel.Render();
        m_TextureSelectionPanel.Render();
        m_TileViewportPanel.Render();
        m_ToolSelectionPanel.Render();
        m_LayerSelectionPanel.Render(); 
    }

    virtual void OnAttach() override
    {
        Tiles::ThemeManager::GetInstance().ApplyDarkTheme();

        // References
        Lumina::Ref<Tiles::Layers> layers = Lumina::CreateRef<Tiles::Layers>();
        layers->Resize(16, 16);

        Lumina::Ref<Tiles::Atlas> atlas = Lumina::CreateRef<Tiles::Atlas>();

        Lumina::Ref<Tiles::ToolModes> modes = Lumina::CreateRef<Tiles::ToolModes>();

        Lumina::Ref<Tiles::State> state = Lumina::CreateRef<Tiles::State>();
        state->SetTileLayers(layers);

        // Header
        m_HeaderPanel.SetTileLayers(layers);
        m_HeaderPanel.SetTextureAtlas(atlas);
        m_HeaderPanel.SetState(state);

        // Viewport
        m_TileViewportPanel.SetTileLayers(layers);
        m_TileViewportPanel.SetTextureAtlas(atlas);
        m_TileViewportPanel.SetToolModes(modes);
		m_TileViewportPanel.SetState(state);

        // Texture Selection
        m_TextureSelectionPanel.SetTextureAtlas(atlas);

        // Layer Selection
        m_LayerSelectionPanel.SetTileLayer(layers);
        m_LayerSelectionPanel.SetState(state);

        // Tool Selection
        m_ToolSelectionPanel.SetToolModes(modes);
    }

    virtual void OnDetach() override
    {
        
    }

private:
    // Panels
    Tiles::HeaderPanel m_HeaderPanel;
    Tiles::TextureSelectionPanel m_TextureSelectionPanel;
    Tiles::TileViewportPanel m_TileViewportPanel;
    Tiles::LayerSelectionPanel m_LayerSelectionPanel;
    Tiles::ToolSelectionPanel m_ToolSelectionPanel;

    // Util
    Lumina::Timer m_FrameTimer;
    float m_FPS = 0;
};