#include "TileSceneRenderer.h"

#include "Domain/Tile.h"
#include "Domain/TileLayer.h"
#include "Domain/TextureAtlas.h"
#include "Graphics/Renderer2D.h"

#include "../App/EditorHost.h"

namespace Tiles::Editor
{
    void DrawTileLayer(
        const Tiles::TileLayer& layer,
        size_t layerIndex,
        float tileSize,
        const std::vector<std::shared_ptr<Tiles::TextureAtlas>>& textureAtlases,
        float baseDepth,
        EditorHost& host)
    {
        // The map holds only painted cells, so every entry is drawn. A tile at
        // signed coord (cx, cy) fills the cell [cx, cx+1] x [cy, cy+1], i.e. it is
        // centered at world ((cx + 0.5) * tileSize, (cy + 0.5) * tileSize).
        for (const auto& [coord, tile] : layer)
        {
            glm::vec2 tileWorldPos = {
                (coord.x + 0.5f) * tileSize,
                (coord.y + 0.5f) * tileSize
            };

            glm::vec2 tileSizeMultiplier = tile.GetSize();

            Tiles::Square params;
            // Nudge each layer's depth so higher layers draw above lower ones.
            params.Position = { tileWorldPos.x, tileWorldPos.y, baseDepth + layerIndex * 0.01f };
            params.Rotation = tile.GetRotation();
            params.Tint = tile.GetTint();
            params.Size = { tileSize * tileSizeMultiplier.x, tileSize * tileSizeMultiplier.y };

            // Untextured tiles keep the defaults (no texture, full coords).
            if (tile.IsTextured() && tile.GetAtlasIndex() < textureAtlases.size())
            {
                auto atlas = textureAtlases[tile.GetAtlasIndex()];
                if (atlas && atlas->HasImage())
                {
                    params.Texture = host.GetAtlasTexture(*atlas);
                    params.TexCoords = tile.GetTextureCoords();
                }
            }

            Tiles::Renderer2D::DrawSquare(params);
        }
    }
}
