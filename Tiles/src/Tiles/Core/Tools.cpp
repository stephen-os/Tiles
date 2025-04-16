#include "Tools.h"

#include "Tile.h"

#include <queue>

#include "../Commands/ReplaceTileCommand.h"
#include "../Commands/ReplaceLayerCommand.h"

namespace Tiles
{
    // This should be made more generic. We should maybe take in a tile we want to 
    // fill with instead of just the index.
    void Tools::Fill(Shared<Layers>& layers, Shared<TextureSelection>& selection, Shared<CommandHistory>& history, size_t y, size_t x)
    {
        size_t layerIndex = layers->GetActiveLayer(); 
        Tile& tile = layers->GetTile(layerIndex, y, x);

        // Check tile to make sure its not the same.
        if (tile.GetTextureIndex() == selection->Front())
            return;

		int newTextureIndex = selection->Front();

        // If the tiles are different, then we ill
        Layer& oldLayer = layers->GetLayer(layerIndex);
        Layer newLayer = oldLayer;

        if (y >= newLayer.GetHeight() || x >= newLayer.GetWidth() || y < 0 || x < 0)
            return;

        int oldTextureIndex = newLayer.GetTile(y, x).GetTextureIndex();

        if (newTextureIndex == oldTextureIndex)
            return;

        std::queue<std::pair<size_t, size_t>> tileQueue;
        tileQueue.push({ y, x });

        const std::vector<std::pair<size_t, size_t>> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

        while (!tileQueue.empty())
        {
            auto [cy, cx] = tileQueue.front();
            tileQueue.pop();

            if (cy >= newLayer.GetHeight() || cx >= newLayer.GetWidth() || cy < 0 || cx < 0)
                continue;

            Tile& tile = newLayer.GetTile(cy, cx);
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

        history->ExecuteCommand(MakeUnique<ReplaceLayerCommand>(layerIndex, oldLayer, newLayer));
    }

    void Tools::Erase(Shared<Layers>& layers, Shared<CommandHistory>& history, size_t y, size_t x)
    {
        size_t layerIndex = layers->GetActiveLayer();
		Tile& oldTile = layers->GetTile(layerIndex, y, x);
		
        // Nothing to errase 
		if (oldTile.GetTextureIndex() == -1)
			return;

		Tile newTile;
		Position position;
		position.L = layerIndex;
		position.Y = y;
		position.X = x;

		history->ExecuteCommand(MakeUnique<ReplaceTileCommand>(position, oldTile, newTile));
    }

	void Tools::Paint(Shared<Layers>& layers, Shared<Lumina::TextureAtlas>& atlas, Shared<TextureSelection>& selection, Shared<CommandHistory>& history, size_t y, size_t x)
	{
        size_t layerIndex = layers->GetActiveLayer();
        Tile& oldTile = layers->GetTile(layerIndex, y, x);

        int newTextureIndex = selection->Front();
        
		if (oldTile.GetTextureIndex() == newTextureIndex)
			return;

        Tile newTile;
		newTile.SetTextureIndex(newTextureIndex);

        Position position;
        position.L = layerIndex;
        position.Y = y;
        position.X = x;

        history->ExecuteCommand(MakeUnique<ReplaceTileCommand>(position, oldTile, newTile));
	}
}