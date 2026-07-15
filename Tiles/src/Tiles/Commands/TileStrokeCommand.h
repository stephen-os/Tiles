#pragma once

#include "Commands/Command.h"

#include "Domain/Tile.h"
#include "Domain/LayerStack.h"

#include <glm/glm.hpp>

#include <utility>
#include <vector>

namespace Tiles
{
	// Applies a set of per-cell tile edits on one layer as a single undo step,
	// capturing the overwritten tiles on first Execute so Undo can restore them.
	// One command covers everything that paints more than one cell at once: a
	// drag stroke, an N x N brush, or a multi-tile stamp (each cell carries its
	// own tile). An empty NewTile erases the cell, so it serves drag-erase too.
	// Building the cell set (stroke path, brush footprint, ...) is the caller's
	// job; this command just applies and reverts it.
	class TileStrokeCommand : public Command
	{
	public:
		// One cell of the stroke: the coordinate and the tile to place there.
		struct Cell
		{
			glm::ivec2 Coord;
			Tile NewTile;
		};

		TileStrokeCommand(size_t layerIndex, std::vector<Cell> cells)
			: m_LayerIndex(layerIndex), m_Cells(std::move(cells)), m_HasExecuted(false)
		{
		}

		// Captures each cell's overwritten tile on first run, then applies the stroke.
		void Execute(LayerStack& layerStack) override
		{
			if (!m_HasExecuted)
			{
				m_PreviousTiles.reserve(m_Cells.size());
				for (const Cell& cell : m_Cells)
					m_PreviousTiles.push_back(layerStack.GetTile(cell.Coord.x, cell.Coord.y, m_LayerIndex));
				m_HasExecuted = true;
			}

			for (const Cell& cell : m_Cells)
				layerStack.SetTile(cell.Coord.x, cell.Coord.y, m_LayerIndex, cell.NewTile);
		}

		// Restores every overwritten tile.
		void Undo(LayerStack& layerStack) override
		{
			for (size_t i = 0; i < m_Cells.size(); ++i)
				layerStack.SetTile(m_Cells[i].Coord.x, m_Cells[i].Coord.y, m_LayerIndex, m_PreviousTiles[i]);
		}

	private:
		size_t m_LayerIndex;
		std::vector<Cell> m_Cells;
		std::vector<Tile> m_PreviousTiles;
		bool m_HasExecuted;
	};
}
