#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace Tiles
{
    class TileLayer;
    class TextureAtlas;
}

namespace Tiles::Editor
{
    class EditorHost;

    // Draws every painted tile of a single layer through Renderer2D. Each tile at
    // signed coord (cx, cy) is centered at world (cx * tileSize, cy * tileSize),
    // with the per-layer depth nudge (baseDepth + layerIndex * 0.01f) and each
    // tile's tint, rotation, and size. Untextured or invalid-atlas tiles reset the
    // quad to no texture and full coords so stale texture state never carries over.
    // @param layer Layer whose painted tiles are drawn.
    // @param layerIndex Position in the stack; scales the per-layer depth nudge.
    // @param tileSize Edge length of one tile in world units.
    // @param textureAtlases Atlases indexed by each tile's atlas index.
    // @param baseDepth Depth of the layer before the per-layer nudge is added.
    // @param host Resolves each atlas to its GL texture for rendering.
    void DrawTileLayer(
        const Tiles::TileLayer& layer,
        size_t layerIndex,
        float tileSize,
        const std::vector<std::shared_ptr<Tiles::TextureAtlas>>& textureAtlases,
        float baseDepth,
        EditorHost& host);
}
