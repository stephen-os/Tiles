#pragma once

#include "Lumina/Renderer/TextureAtlas.h"
#include "../Core/ToolModes.h"
#include "../Core/TileLayer.h"

#include "imgui.h"

class TileViewportPanel {
public:
    void Render(int selectedTexture);

    // Setters
    void SetTextureAtlas(Lumina::TextureAtlas& atlas) { m_TextureAtlas = &atlas; }
    void SetTileLayer(TileLayer& layers) { m_Layers = &layers; }
    void SetToolModes(ToolModes& modes) { m_Modes = &modes; }

private:
    // UI Rendering Methods
    void RenderTileGrid();
    void RenderLayerTiles(int selectedTexture);
    void HandleTileSelection(int layer, int x, int y, int selectedTexture);
    void DrawTileTexture(const TileData& tile, ImVec2 tileMin, ImVec2 tileMax);
    void HandleScrolling();

private:
    float m_Zoom = 1.0f;

    TileLayer* m_Layers = nullptr;
    Lumina::TextureAtlas* m_TextureAtlas = nullptr;
    ToolModes* m_Modes = nullptr;

    // Constants
    static constexpr float TILE_SIZE = 40.0f;
    static constexpr ImU32 BACKGROUND_COLOR = IM_COL32(77, 77, 77, 255);        // Dark Gray
    static constexpr ImU32 OUTLINE_COLOR = IM_COL32(169, 169, 169, 255);        // Gray
    static constexpr ImU32 SELECTION_BORDER_COLOR = IM_COL32(255, 165, 0, 255); // Orange
    static constexpr ImU32 FILL_COLOR = IM_COL32(255, 255, 255, 255);           // White
};
