#pragma once

#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Base.h"

#include "../Core/ToolModes.h"
#include "../Core/TileLayer.h"

#include "imgui.h"

class TileViewportPanel {
public:
    void Render(int selectedTexture);

    // Setters
    void SetTextureAtlas(const Lumina::Ref<Lumina::TextureAtlas>& atlas) { m_TextureAtlas = atlas; }
    void SetTileLayers(const Lumina::Ref<TileLayer>& layers) { m_TileLayers = layers; }
    void SetToolModes(const Lumina::Ref<ToolModes>& modes) { m_ToolModes = modes; }

private:
    // UI Rendering Methods
    void RenderTileGrid();
    void RenderLayerTiles(int selectedTexture);
    void HandleTileSelection(int layer, int x, int y, int selectedTexture);
    void DrawTileTexture(const TileData& tile, ImVec2 tileMin, ImVec2 tileMax);
    void HandleScrolling();

private:
    float m_Zoom = 1.0f;

    Lumina::Ref<TileLayer> m_TileLayers;
    Lumina::Ref<Lumina::TextureAtlas> m_TextureAtlas;
    Lumina::Ref<ToolModes> m_ToolModes;

    // Constants
    static constexpr float TILE_SIZE = 40.0f;
    static constexpr float CHECKERBOARD_SIZE = 10.0f;
    static constexpr ImU32 BACKGROUND_COLOR = IM_COL32(77, 77, 77, 255);        // Dark Gray
    static constexpr ImU32 OUTLINE_COLOR = IM_COL32(169, 169, 169, 255);        // Gray
    static constexpr ImU32 SELECTION_BORDER_COLOR = IM_COL32(255, 165, 0, 255); // Orange
    static constexpr ImU32 FILL_COLOR = IM_COL32(255, 255, 255, 255);           // White

    static constexpr ImU32 CHECKERBOARD_COLOR_1 = IM_COL32(120, 120, 120, 255); // Dark gray
    static constexpr ImU32 CHECKERBOARD_COLOR_2 = IM_COL32(80, 80, 80, 255);    // Even darker gray
};
