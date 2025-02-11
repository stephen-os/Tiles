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
#include "Panels/AttributePanel.h"
#include "Panels/ToolSelectionPanel.h"

#include "Core/ToolModes.h"
#include "Core/TileLayer.h"

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
        m_TileViewportPanel.Render(m_TextureSelectionPanel.GetSelectedTexture());
        m_ToolSelectionPanel.Render();
        m_LayerSelectionPanel.Render();
        m_AttributePanel.Render(); 
    }

    virtual void OnAttach() override
    {
        ThemeManager::GetInstance().ApplyDarkTheme();

        // References
        Lumina::Ref<TileLayer> layers = Lumina::CreateRef<TileLayer>();
        Lumina::Ref<Lumina::TextureAtlas> atlas = Lumina::CreateRef<Lumina::TextureAtlas>();
        Lumina::Ref<ToolModes> modes = Lumina::CreateRef<ToolModes>();

        // TODO: Only init the grid from new or load
        layers->Create(25, 25);

        // Header
        m_HeaderPanel.SetTileLayers(layers);
        m_HeaderPanel.SetTextureAtlas(atlas);

        // Viewport
        m_TileViewportPanel.SetTileLayers(layers);
        m_TileViewportPanel.SetTextureAtlas(atlas);
        m_TileViewportPanel.SetToolModes(modes);

        // Texture Selection
        m_TextureSelectionPanel.SetTextureAtlas(atlas);

        // Layer Selection
        m_LayerSelectionPanel.SetTileLayer(layers);

        // Tile Attribute
        m_AttributePanel.SetTileLayers(layers);
        m_AttributePanel.SetTextureAtlas(atlas);

        // Tool Selection
        m_ToolSelectionPanel.SetToolModes(modes);
    }

    virtual void OnDetach() override
    {
        
    }

private:
    // Panels
    HeaderPanel m_HeaderPanel;
    TextureSelectionPanel m_TextureSelectionPanel;
    TileViewportPanel m_TileViewportPanel;
	LayerSelectionPanel m_LayerSelectionPanel;
    AttributePanel m_AttributePanel;
    ToolSelectionPanel m_ToolSelectionPanel;

    // Util
    Lumina::Timer m_FrameTimer;
    float m_FPS = 0;
};