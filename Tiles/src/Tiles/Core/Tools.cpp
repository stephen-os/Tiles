#include "Tools.h"

#include "Tile.h"

#include <queue>
#include <iostream>

namespace Tiles
{

    // This should be made more generic. We should maybe take in a tile we want to 
    // fill with instead of just the index.
    void Tools::Fill(Layer& layer, int newTextureIndex, size_t y, size_t x)
    {
        if (y >= layer.GetHeight() || x >= layer.GetWidth() || y < 0 || x < 0)
            return;

        int oldTextureIndex = layer.GetTile(y, x).GetTextureIndex();

        if (newTextureIndex == oldTextureIndex)
            return;

        std::queue<std::pair<size_t, size_t>> tileQueue;
        tileQueue.push({ y, x });

        const std::vector<std::pair<size_t, size_t>> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

        while (!tileQueue.empty())
        {
            auto [cy, cx] = tileQueue.front();
            tileQueue.pop();

            if (cy >= layer.GetHeight() || cx >= layer.GetWidth() || cy < 0 || cx < 0)
                continue;

            Tile& tile = layer.GetTile(cy, cx);
            if (tile.GetTextureIndex() != oldTextureIndex)
                continue;

            tile.SetTextureIndex(newTextureIndex);

            for (const auto& [dy, dx] : directions)
            {
                size_t ny = cy + dy;
                size_t nx = cx + dx;

                tileQueue.push({ ny, nx });
            }
        }
    }

}