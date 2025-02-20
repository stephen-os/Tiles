#pragma once

#include "Lumina/Renderer/VertexArray.h"
#include "Lumina/Renderer/ShaderProgram.h"


#include "../Core/TextureSelection.h"
#include "../Core/ToolModes.h"
#include "../Core/Atlas.h"
#include "../Core/Layers.h"
#include "../Core/Layer.h"
#include "../Core/Tile.h"
#include "../Core/State.h"
#include "../Core/Base.h"
#include "../Core/Camera.h"
#include "../Core/Quad.h"

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
        void SetTextureAtlas(const Shared<Atlas>& atlas) { m_Atlas = atlas; }
        void SetTileLayers(const Shared<Layers>& layers) { m_Layers = layers; }
        void SetToolModes(const Shared<ToolModes>& modes) { m_ToolModes = modes; }
        void SetState(const Shared<State>& state) { m_State = state; }
        void SetSelection(const Shared<TextureSelection>& selection) { m_Selection = selection; }
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
        Shared<ToolModes> m_ToolModes;
        Shared<State> m_State;
        Shared<TextureSelection> m_Selection;

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