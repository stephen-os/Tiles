#pragma once

#include "Command.h"

#include "../Core/Tile.h"
#include "../Core/Layers.h"

#include "Lumina/Core/Log.h"

namespace Tiles
{
	class PaintTileCommand : public Command
	{
	public:
		PaintTileCommand(const TilePosition& position, const Layers& layers, const TileAttributes& tileAttributes)
		{
			m_Position = position;
			m_PreviousTile = layers.GetTile(position.LayerIndex, position.RowIndex, position.ColIndex); 
			m_NewTile = tileAttributes.GetTile();
		}

		virtual void Execute(Layers& layers) override
		{
			Tile& currentTile = layers.GetTile(m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex);
			
			// Dont execute if the tile is already the same
			if (currentTile == m_NewTile)
				return;

			LUMINA_LOG_INFO("Position: ({}, {}, {}) Exectute Paint {} With {}", m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex, currentTile.ToString(), m_NewTile.ToString());
			currentTile = m_NewTile;
		}

		virtual void Undo(Layers& layers) override
		{
			Tile& currentTile = layers.GetTile(m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex);
			LUMINA_LOG_INFO("Position: ({}, {}, {}) Undo Paint {} With {}", m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex, currentTile.ToString(), m_PreviousTile.ToString());
			currentTile = m_PreviousTile;
		}

		virtual bool Validate(const Command& other) const override
		{
			const PaintTileCommand* otherCmd = dynamic_cast<const PaintTileCommand*>(&other);
			if (!otherCmd)
				return false;

			return m_Position == otherCmd->m_Position && m_NewTile == otherCmd->m_NewTile;
		}

	private:
		TilePosition m_Position;
		Tile m_PreviousTile;
		Tile m_NewTile;
	};
}