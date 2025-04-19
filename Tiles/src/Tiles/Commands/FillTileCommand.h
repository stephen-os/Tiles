#pragma once

#include "../Core/Tile.h"

#include "Lumina/Core/Log.h"

#include "Command.h"

#include <queue>

namespace Tiles
{
	class FillTileCommand : public Command
	{
	public:
		FillTileCommand(TilePosition& position, Tile& newTile)
		{
			m_Position = position;
			m_FillTile = newTile;
		}

		virtual void Execute(Layers& layers) override
		{
			m_OldLayer = layers.GetLayer(m_Position.LayerIndex);
			m_NewLayer = m_OldLayer;

			Tile& oldTile = m_NewLayer.GetTile(m_Position.RowIndex, m_Position.ColIndex);
			int oldTextureIndex = oldTile.GetTextureIndex();

			std::queue<std::pair<size_t, size_t>> tileQueue;
			tileQueue.push({ m_Position.RowIndex, m_Position.ColIndex });

			const std::vector<std::pair<size_t, size_t>> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

			while (!tileQueue.empty())
			{
				auto [cy, cx] = tileQueue.front();
				tileQueue.pop();

				if (cy >= m_NewLayer.GetHeight() || cx >= m_NewLayer.GetWidth() || cy < 0 || cx < 0)
					continue;

				Tile& tile = m_NewLayer.GetTile(cy, cx);
				if (tile.GetTextureIndex() != oldTextureIndex)
					continue;

				tile.SetTextureIndex(m_FillTile.GetTextureIndex());

				for (const auto& [dy, dx] : directions)
				{
					size_t ny = cy + dy;
					size_t nx = cx + dx;

					tileQueue.push({ ny, nx });
				}
			}
			
			LUMINA_LOG_INFO("Position: ({}, {}, {}) Execute Fill {} With {}", m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex, oldTextureIndex, m_FillTile.GetTextureIndex());
			layers.ReplaceLayer(m_Position.LayerIndex, m_NewLayer);
		}

		void Undo(Layers& layers) override
		{
			Tile& oldTile = m_OldLayer.GetTile(m_Position.RowIndex, m_Position.ColIndex);
			LUMINA_LOG_INFO("Position: ({}, {}, {}) Undo Fill {} With {}", m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex, m_FillTile.GetTextureIndex(), oldTile.GetTextureIndex());
			layers.ReplaceLayer(m_Position.LayerIndex, m_OldLayer);
		}

		virtual bool Validate(const Command& other) const override
		{
			return false; 
		}
	private:
		TilePosition m_Position;

		Tile m_FillTile;
		Layer m_OldLayer;
		Layer m_NewLayer;
	};
}