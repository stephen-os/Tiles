#pragma once

#include "Command.h"

#include "../Core/Tile.h"
#include "../Core/Layers.h"

#include "Lumina/Core/Log.h"

namespace Tiles
{
	class ReplaceTileCommand : public Command
	{
	public:
		ReplaceTileCommand(const TilePosition& position, const Tile& oldTile, const Tile& newTile)
		{
			m_Position = position;
			m_PreviousTile = oldTile; 
			m_NewTile = newTile;
		}

		virtual void Execute(Layers& layers) override
		{
			Tile& curretTile = layers.GetTile(m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex);
			LUMINA_LOG_INFO("Position: ({}, {}, {}) Exectute Replace {} With {}", m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex, curretTile.GetTextureIndex(), m_NewTile.GetTextureIndex());
			curretTile = m_NewTile;
		}

		virtual void Undo(Layers& layers) override
		{
			Tile& curretTile = layers.GetTile(m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex);
			LUMINA_LOG_INFO("Position: ({}, {}, {}) Undo Replace {} With {}", m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex, curretTile.GetTextureIndex(), m_PreviousTile.GetTextureIndex());
			curretTile = m_PreviousTile;
		}

		virtual bool Validate(const Command& other) const override
		{
			const ReplaceTileCommand* otherCmd = dynamic_cast<const ReplaceTileCommand*>(&other);
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