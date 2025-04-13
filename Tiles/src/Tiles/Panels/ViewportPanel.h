#pragma once

#include "Lumina/Renderer/VertexArray.h"
#include "Lumina/Renderer/ShaderProgram.h"
#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Renderer/Cameras/OrthographicCamera.h"
#include "Lumina/Renderer/Renderer.h"

#include "../Core/TextureSelection.h"
#include "../Core/ToolSelection.h"
#include "../Core/Layers.h"
#include "../Core/Layer.h"
#include "../Core/Tile.h"
#include "../Core/Base.h"
#include "../Core/Camera.h"

#include "../Commands/CommandHistory.h"

#include "imgui.h"

namespace Tiles
{

    class ViewportPanel
    {
    public:
        ViewportPanel();
        ~ViewportPanel() = default;

        void OnUIRender();

        // Setters
        void SetLayers(const Shared<Layers>& layers) { m_Layers = layers; }
        void SetTextureAtlas(const Shared<Lumina::TextureAtlas>& atlas) { m_Atlas = atlas; m_TileAttributes.Texture = atlas->GetTexture(); }
        void SetToolSelection(const Shared<ToolSelection>& toolSelection) { m_ToolSelection = toolSelection; }
        void SetTextureSelection(const Shared<TextureSelection>& textureSelection) { m_TextureSelection = textureSelection; }
        void SetCommandHistory(const Shared<CommandHistory>& history) { m_CommandHistory = history; }
    private:
        Shared<Layers> m_Layers;
        Shared<Lumina::TextureAtlas> m_Atlas;
        Shared<ToolSelection> m_ToolSelection;
        Shared<TextureSelection> m_TextureSelection;
        Shared<CommandHistory> m_CommandHistory;

        // Background
		Lumina::QuadAttributes m_BackgroundAttributes;
		Lumina::QuadAttributes m_TileAttributes;

        Lumina::OrthographicCamera m_ViewportCamera; 

        // Ui State
        bool m_IsMouseDragging = false;
        bool m_IsMiddleMouseDown = false;
        bool m_ProcessClick = false;

        glm::vec2 m_LastMousePos = { 0.0f, 0.0f };
        glm::vec2 m_LastTilePos = { 0.0f, 0.0f };

        // Viewport Specifications
        glm::vec2 m_ViewportSize = { 1000.0f, 1000.0f };
        float m_TileSize = 40.0f;
    };

}