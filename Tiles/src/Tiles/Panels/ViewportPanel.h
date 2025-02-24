#pragma once

#include "Lumina/Renderer/VertexArray.h"
#include "Lumina/Renderer/ShaderProgram.h"


#include "../Core/TextureSelection.h"
#include "../Core/ToolSelection.h"
#include "../Core/Atlas.h"
#include "../Core/Layers.h"
#include "../Core/Layer.h"
#include "../Core/Tile.h"
#include "../Core/Base.h"
#include "../Core/Camera.h"
#include "../Core/Quad.h"

#include "../Commands/CommandHistory.h"
#include "../Commands/PaintCommand.h"
#include "../Commands/FillCommand.h"

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
        void SetTextureAtlas(const Shared<Atlas>& atlas) { m_Atlas = atlas; }
        void SetToolSelection(const Shared<ToolSelection>& toolSelection) { m_ToolSelection = toolSelection; }
        void SetTextureSelection(const Shared<TextureSelection>& textureSelection) { m_TextureSelection = textureSelection; }
        void SetCommandHistory(const Shared<CommandHistory>& history) { m_CommandHistory = history; }
    private:
        // UI Rendering
        void RenderBackground();
        void RenderTiles();

        // Drawing
        void DrawHoveredTile(ImVec2 tileMin, ImVec2 tileMax, size_t l, size_t y, size_t x);
        void DrawTile(ImVec2 tileMin, ImVec2 tileMax, size_t l, size_t y, size_t x);

        // Input Handling 
        void HandleSelection(size_t l, size_t y, size_t x);
        void HandleMouseInput();

        // Utilities
        bool IsNewClick();
        bool IsNewTileDuringDrag(glm::vec2 currentTilePos);
        bool IsMouseInViewport(const ImVec2& mousePos, const ImVec2& windowPos, const ImVec2& windowSize);
    private:
        Shared<Layers> m_Layers;
        Shared<Atlas> m_Atlas;
        Shared<ToolSelection> m_ToolSelection;
        Shared<TextureSelection> m_TextureSelection;
        Shared<CommandHistory> m_CommandHistory;

        // Rendering
        Quad m_Background;
        Camera m_Camera;

        // Ui State
        bool m_IsMouseDragging = false;
        bool m_IsMiddleMouseDown = false;
        bool m_ProcessClick = false;

        glm::vec2 m_LastMousePos = { 0.0f, 0.0f };
        glm::vec2 m_LastTilePos = { 0.0f, 0.0f };

        // Viewport Specifications
        glm::vec2 m_ViewportSize = { 1200.0f, 1200.0f };
        float m_TileSize = 40.0f;
    };

}