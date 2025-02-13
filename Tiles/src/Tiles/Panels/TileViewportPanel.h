#pragma once

#include "Lumina/Base.h"

#include "../Core/ToolModes.h"
#include "../Core/Atlas.h"
#include "../Core/Layers.h"
#include "../Core/Layer.h"
#include "../Core/Tile.h"

#include "imgui.h"

class TileViewportPanel
{
public:
    TileViewportPanel() = default;
    ~TileViewportPanel() = default;

    void Render();

    // Setters
    void SetTextureAtlas(const Lumina::Ref<Atlas>& atlas) { m_Atlas = atlas; }
    void SetTileLayers(const Lumina::Ref<Layers>& layers) { m_Layers = layers; }
    void SetToolModes(const Lumina::Ref<ToolModes>& modes) { m_ToolModes = modes; }

private:
    // UI Rendering Methods
    void RenderBackground();
    void RenderTiles();
    void HandleTileSelection(Layer& layer, Tile& tile, size_t y, size_t x);
    void DrawTile(const Tile& tile, ImVec2 tileMin, ImVec2 tileMax);
    void HandleInput();

private:
    float m_Zoom = 1.0f;
    ImVec2 m_MousePosition = ImVec2(0.0f, 0.0f);

    Lumina::Ref<Layers> m_Layers;
    Lumina::Ref<Atlas> m_Atlas;
    Lumina::Ref<ToolModes> m_ToolModes;

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
