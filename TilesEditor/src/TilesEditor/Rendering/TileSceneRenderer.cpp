#include "TileSceneRenderer.h"

#include "Core/Tile.h"
#include "Core/TileLayer.h"
#include "Graphics/Renderer2D.h"
#include "Graphics/TextureAtlas.h"

namespace Tiles::Editor
{
    void DrawTileLayer(
        const Tiles::TileLayer& layer,
        size_t layerIndex,
        const glm::vec3& cameraPos,
        float tileSize,
        const std::vector<std::shared_ptr<Tiles::TextureAtlas>>& textureAtlases,
        float baseDepth,
        bool resetTextureFallback)
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
                    (x + 1) * tileSize + cameraPos.x,
                    (y + 1) * tileSize + cameraPos.y
                };

                // Nudge each layer's depth so higher layers draw above lower ones.
                Tiles::Renderer2D::SetQuadPosition({
                    tileWorldPos.x,
                    tileWorldPos.y,
                    baseDepth + layerIndex * 0.01f
                    });
                Tiles::Renderer2D::SetQuadRotation(tile.GetRotation());
                Tiles::Renderer2D::SetQuadTintColor(tile.GetTint());

                glm::vec2 tileSizeMultiplier = tile.GetSize();
                Tiles::Renderer2D::SetQuadSize({
                    tileSize * tileSizeMultiplier.x,
                    tileSize * tileSizeMultiplier.y
                    });

                if (tile.IsTextured() && tile.GetAtlasIndex() < textureAtlases.size())
                {
                    auto atlas = textureAtlases[tile.GetAtlasIndex()];
                    if (atlas && atlas->HasTexture())
                    {
                        Tiles::Renderer2D::SetQuadTexture(atlas->GetTexture());
                        Tiles::Renderer2D::SetQuadTextureCoords(tile.GetTextureCoords());
                    }
                    else if (resetTextureFallback)
                    {
                        Tiles::Renderer2D::SetQuadTexture(nullptr);
                        Tiles::Renderer2D::SetQuadTextureCoords({ 0.0f, 0.0f, 1.0f, 1.0f });
                    }
                }
                else
                {
                    Tiles::Renderer2D::SetQuadTexture(nullptr);
                    if (resetTextureFallback)
                    {
                        Tiles::Renderer2D::SetQuadTextureCoords({ 0.0f, 0.0f, 1.0f, 1.0f });
                    }
                }

                Tiles::Renderer2D::DrawQuad();
            }
        }
    }
}
