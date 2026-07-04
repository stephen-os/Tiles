#include "TileSceneRenderer.h"

#include "Domain/Tile.h"
#include "Domain/TileLayer.h"
#include "Graphics/Renderer2D.h"
#include "Graphics/TextureAtlas.h"

namespace Tiles::Editor
{
    void DrawTileLayer(
        const Tiles::TileLayer& layer,
        size_t layerIndex,
        float tileSize,
        const std::vector<std::shared_ptr<Tiles::TextureAtlas>>& textureAtlases,
        float baseDepth)
    {
        // The map holds only painted cells, so every entry is drawn. A tile at
        // signed coord (cx, cy) is centered at world (cx * tileSize, cy * tileSize).
        for (const auto& [coord, tile] : layer)
        {
            glm::vec2 tileWorldPos = {
                coord.x * tileSize,
                coord.y * tileSize
            };

            glm::vec2 tileSizeMultiplier = tile.GetSize();

            Tiles::QuadParams params;
            // Nudge each layer's depth so higher layers draw above lower ones.
            params.Position = { tileWorldPos.x, tileWorldPos.y, baseDepth + layerIndex * 0.01f };
            params.Rotation = tile.GetRotation();
            params.Tint = tile.GetTint();
            params.Size = { tileSize * tileSizeMultiplier.x, tileSize * tileSizeMultiplier.y };

            // Untextured tiles keep the defaults (no texture, full coords).
            if (tile.IsTextured() && tile.GetAtlasIndex() < textureAtlases.size())
            {
                auto atlas = textureAtlases[tile.GetAtlasIndex()];
                if (atlas && atlas->HasTexture())
                {
                    params.Texture = atlas->GetTexture();
                    params.TexCoords = tile.GetTextureCoords();
                }
            }

            Tiles::Renderer2D::DrawQuad(params);
        }
    }
}
