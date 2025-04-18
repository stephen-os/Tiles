#pragma once

#include "Lumina/Renderer/Cameras/OrthographicCamera.h"
#include "Lumina/Renderer/Renderer.h"
#include "Lumina/Renderer/ShaderProgram.h"
#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Renderer/VertexArray.h"

#include "Lumina/Core/Aliases.h"

#include "../Core/TextureSelection.h"
#include "../Core/Layers.h"
#include "../Core/Layer.h"
#include "../Core/Tile.h"
#include "../Core/Camera.h"

#include "../Commands/CommandHistory.h"

#include "imgui.h"

namespace Tiles
{

    class ViewportPanel
    {
    public:
        ViewportPanel() = default;
        ~ViewportPanel() = default;

        void OnUIRender();

        // Setters
        void SetLayers(const Shared<Layers>& layers) { m_Layers = layers; }
        void SetTextureAtlas(const Shared<Lumina::TextureAtlas>& atlas) { m_Atlas = atlas; }
        void SetTextureSelection(const Shared<TextureSelection>& textureSelection) { m_TextureSelection = textureSelection; }
        void SetCommandHistory(const Shared<CommandHistory>& history) { m_CommandHistory = history; }
    private:
        void RenderOverlay();

        void HandleSelection(size_t y, size_t x);
		void HandleInput();
    private:
        Shared<Layers> m_Layers;
        Shared<Lumina::TextureAtlas> m_Atlas;
        Shared<TextureSelection> m_TextureSelection;
        Shared<CommandHistory> m_CommandHistory;

        // Ui State
        bool m_IsMouseDragging = false;
        bool m_IsMiddleMouseDown = false;
        bool m_ProcessClick = false;

        glm::vec2 m_CursorPos = { 60.0f, 60.0f }; 

        glm::vec2 m_LastMousePos = { 0.0f, 0.0f };
        glm::vec2 m_LastTilePos = { 0.0f, 0.0f };

        // Viewport Specifications
        glm::vec2 m_ViewportSize = { 1200.0f, 1200.0f };
        float m_TileSize = 480.0f;
        float m_Zoom = 10.0f;
    };

}