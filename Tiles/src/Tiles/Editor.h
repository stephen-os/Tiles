#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Lumina/Layer.h"

#include "Lumina/Utils/Timer.h"
#include "Lumina/Utils/FileReader.h"

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

        // Default Tile Layer
        m_TileLayer.Create(25, 25); 

        // Header
        m_HeaderPanel.SetTileLayer(m_TileLayer);
        m_HeaderPanel.SetTextureAtlas(m_TextureAtlas);

        // Viewport
        m_TileViewportPanel.SetTileLayer(m_TileLayer);
        m_TileViewportPanel.SetTextureAtlas(m_TextureAtlas);
        m_TileViewportPanel.SetToolModes(m_ToolModes);

        // Texture Selection
        m_TextureSelectionPanel.SetTextureAtlas(m_TextureAtlas);

        // Layer Selection
        m_LayerSelectionPanel.SetTileLayer(m_TileLayer);

        // Tile Attribute
        m_AttributePanel.SetTileLayer(m_TileLayer);
        m_AttributePanel.SetTextureAtlas(m_TextureAtlas);

        // Tool Selection
        m_ToolSelectionPanel.SetToolModes(m_ToolModes);
    }

    virtual void OnDetach() override
    {
        
    }

private:
    // Common
    TileLayer m_TileLayer;
    Lumina::TextureAtlas m_TextureAtlas;
    ToolModes m_ToolModes;

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