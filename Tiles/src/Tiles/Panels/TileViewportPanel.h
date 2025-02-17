#pragma once

#include "../Core/Selection.h"
#include "../Core/ToolModes.h"
#include "../Core/Atlas.h"
#include "../Core/Layers.h"
#include "../Core/Layer.h"
#include "../Core/Tile.h"
#include "../Core/State.h"
#include "../Core/Base.h"

#include "imgui.h"

namespace Tiles
{

    class TileViewportPanel
    {
    public:
        TileViewportPanel() = default;
        ~TileViewportPanel() = default;

        void Render();

        // Setters
        void SetTextureAtlas(const Shared<Atlas>& atlas) { m_Atlas = atlas; }
        void SetTileLayers(const Shared<Layers>& layers) { m_Layers = layers; }
        void SetToolModes(const Shared<ToolModes>& modes) { m_ToolModes = modes; }
        void SetState(const Shared<State>& state) { m_State = state; }
        void SetSelection(const Shared<Selection>& selection) { m_Selection = selection; }
    private:
        // UI Rendering Methods
        void RenderBackground();
        void RenderTiles();
        // Drawing Methods
        void DrawHoveredTile(ImVec2 tileMin, ImVec2 tileMax, size_t l, size_t y, size_t x);
        void DrawTile(ImVec2 tileMin, ImVec2 tileMax, size_t l, size_t y, size_t x);
        
        // Input Handling 
        void HandleInput();
        void HandleSelection(size_t l, size_t y, size_t x);

        // Utilities
        bool IsNewClick(); 
        bool IsNewTileDuringDrag(ImVec2 currentTilePos);
    private:
        float m_Zoom = 1.0f;
        bool m_IsMouseDragging = false;  // Track if we're in the middle of a drag
        ImVec2 m_LastMousePosition = ImVec2(-1, -1);  // Track last modified tile position

        Shared<Layers> m_Layers;
        Shared<Atlas> m_Atlas;
        Shared<ToolModes> m_ToolModes;
        Shared<State> m_State;
        Shared<Selection> m_Selection;

        // Constants
        static constexpr float TILE_SIZE = 40.0f;
        static constexpr float CHECKERBOARD_SIZE = 10.0f;
        static constexpr ImU32 BACKGROUND_COLOR = IM_COL32(77, 77, 77, 255);
        static constexpr ImU32 OUTLINE_COLOR = IM_COL32(169, 169, 169, 255);
        static constexpr ImU32 SELECTION_BORDER_COLOR = IM_COL32(255, 165, 0, 255);
        static constexpr ImU32 FILL_COLOR = IM_COL32(255, 255, 255, 255);

        static constexpr ImU32 CHECKERBOARD_COLOR_1 = IM_COL32(120, 120, 120, 255);
        static constexpr ImU32 CHECKERBOARD_COLOR_2 = IM_COL32(80, 80, 80, 255);
    };

}