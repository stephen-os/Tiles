#pragma once

#include "Command.h"

#include "../Core/Tile.h"
#include "../Core/Layers.h"
#include <iostream>

namespace Tiles
{
	class PaintCommand : public Command
	{
	public:
		PaintCommand(const Position& position, const Tile& oldTile, const Tile& newTile)
		{
			m_Position = position;
			m_PreviousTile = oldTile; 
			m_NewTile = newTile;
		}

		virtual void Execute(Layers& layers) override
		{
			Tile& curretTile = layers.GetTile(m_Position.index, m_Position.y, m_Position.x);
			curretTile = m_NewTile;
		}

		virtual void Undo(Layers& layers) override
		{
			Tile& curretTile = layers.GetTile(m_Position.index, m_Position.y, m_Position.x);
			curretTile = m_PreviousTile;
		}
	private:
		Position m_Position;
		Tile m_PreviousTile;
		Tile m_NewTile;
	};
}