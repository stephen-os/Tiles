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
        for (size_t y = 0; y < layer.GetHeight(); ++y)
        {
            for (size_t x = 0; x < layer.GetWidth(); ++x)
            {
                const Tile& tile = layer.GetTile(x, y);
                if (!tile.IsPainted()) continue;

                // The (x + 1, y + 1) offset matches the viewport's one-tile border,
                // keeping exported output aligned with what the editor shows.
                glm::vec2 tileWorldPos = {
                    (x + 1) * tileSize,
                    (y + 1) * tileSize
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
}
