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
    /// Draws every painted tile of a single layer through Renderer2D, applying the
    /// viewport's (x + 1, y + 1) one-tile border, the per-layer depth nudge
    /// (baseDepth + layerIndex * 0.01f), and each tile's tint, rotation, and size.
    /// @param layer Layer whose painted tiles are drawn.
    /// @param layerIndex Position in the stack; scales the per-layer depth nudge.
    /// @param cameraPos World-space camera offset added to every tile position.
    /// @param tileSize Edge length of one tile in world units.
    /// @param textureAtlases Atlases indexed by each tile's atlas index.
    /// @param baseDepth Depth of the layer before the per-layer nudge is added.
    /// @param resetTextureFallback When true (viewport), untextured or invalid-atlas
    ///        tiles also reset the quad's texture coords, and an invalid atlas clears
    ///        the quad texture; when false (export), those fallbacks are skipped so
    ///        each call site keeps its exact original behavior.
    void DrawTileLayer(
        const Tiles::TileLayer& layer,
        size_t layerIndex,
        const glm::vec3& cameraPos,
        float tileSize,
        const std::vector<std::shared_ptr<Tiles::TextureAtlas>>& textureAtlases,
        float baseDepth,
        bool resetTextureFallback);
}
