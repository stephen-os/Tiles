#pragma once

#include "Command.h"

#include "../Core/Tile.h"
#include "../Core/Layers.h"

#include "Lumina/Core/Log.h"

namespace Tiles
{
	class EraseTileCommand : public Command
	{
	public:
		EraseTileCommand(const TilePosition& position, const Layers& layers)
		{
			m_Position = position;
			m_PreviousTile = layers.GetTile(position.LayerIndex, position.RowIndex, position.ColIndex);
		}

		virtual void Execute(Layers& layers) override
		{
			Tile& curretTile = layers.GetTile(m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex);

			Tile emptyTile;

			if (curretTile == emptyTile)
				return;

			LUMINA_LOG_INFO("Position: ({}, {}, {}) Erase {} with {}", m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex, emptyTile.ToString(), curretTile.ToString());
			curretTile.Reset();
		}

		virtual void Undo(Layers& layers) override
		{
			Tile& curretTile = layers.GetTile(m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex);
			LUMINA_LOG_INFO("Position: ({}, {}, {}) Undo Erase {} With {}", m_Position.LayerIndex, m_Position.RowIndex, m_Position.ColIndex, curretTile.ToString(), m_PreviousTile.ToString());
			curretTile = m_PreviousTile;
		}

		virtual bool Validate(const Command& other) const override
		{
			const EraseTileCommand* otherCmd = dynamic_cast<const EraseTileCommand*>(&other);
			if (!otherCmd)
				return false;

			return m_Position == otherCmd->m_Position;
		}

	private:
		TilePosition m_Position;
		Tile m_PreviousTile;
	};
}