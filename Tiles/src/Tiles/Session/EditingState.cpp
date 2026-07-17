#include "EditingState.h"

#include "Domain/LayerStack.h"
#include "Commands/TilePaintCommand.h"
#include "Commands/TileEraseCommand.h"
#include "Commands/LayerFillCommand.h"
#include "Commands/TileStrokeCommand.h"

namespace Tiles
{
	// Restores the default selection: layer 0, no mode, a painted brush.
	void EditingState::Reset()
	{
		m_WorkingLayer = 0;
		m_PaintingMode = PaintingMode::None;
		m_Brush = Tile();
		m_Brush.SetPainted(true);
	}

	// Clamps the working layer to the last valid index after the layer count shrinks.
	void EditingState::ValidateWorkingLayer(const LayerStack& layerStack)
	{
		if (layerStack.IsEmpty())
			m_WorkingLayer = 0;
		else if (m_WorkingLayer >= layerStack.GetLayerCount())
			m_WorkingLayer = layerStack.GetLayerCount() - 1;
	}

	// Builds the command matching the active painting mode; null for None/Fill.
	std::unique_ptr<Command> EditingState::BuildModeCommand(size_t layerIndex, int x, int y, const Tile& tile) const
	{
		switch (m_PaintingMode)
		{
		case PaintingMode::Brush:
			return std::make_unique<TilePaintCommand>(x, y, layerIndex, tile);
		case PaintingMode::Eraser:
			return std::make_unique<TileEraseCommand>(x, y, layerIndex);
		// Fill needs the visible-view bound, so it goes through Session::FillLayer
		// / BuildFillCommand rather than this per-cell dispatch.
		default:
			return nullptr;
		}
	}

	// Builds an erase command for the cell at (x, y).
	std::unique_ptr<Command> EditingState::BuildEraseCommand(size_t layerIndex, int x, int y) const
	{
		return std::make_unique<TileEraseCommand>(x, y, layerIndex);
	}

	// Builds a flood-fill command bounded by the visible region.
	std::unique_ptr<Command> EditingState::BuildFillCommand(size_t layerIndex, int x, int y, const Tile& tile, const glm::ivec4& bounds) const
	{
		return std::make_unique<LayerFillCommand>(x, y, layerIndex, tile, bounds);
	}

	// The N x N block of cells centered on (cx, cy) for the current brush size.
	std::vector<glm::ivec2> EditingState::BrushFootprint(int cx, int cy) const
	{
		std::vector<glm::ivec2> cells;
		cells.reserve(static_cast<size_t>(m_BrushSize) * m_BrushSize);

		const int half = m_BrushSize / 2;
		for (int dy = 0; dy < m_BrushSize; ++dy)
			for (int dx = 0; dx < m_BrushSize; ++dx)
				cells.push_back({ cx - half + dx, cy - half + dy });

		return cells;
	}

	// Builds a stroke that paints (or erases) every cell as one undo step.
	std::unique_ptr<Command> EditingState::BuildStrokeCommand(size_t layerIndex, const std::vector<glm::ivec2>& cells) const
	{
		// Brush stamps the brush tile; eraser stamps an empty tile (which clears
		// the cell). Other modes do not stroke.
		Tile tile;
		if (m_PaintingMode == PaintingMode::Brush)
			tile = m_Brush;
		else if (m_PaintingMode != PaintingMode::Eraser)
			return nullptr;

		if (cells.empty())
			return nullptr;

		std::vector<TileStrokeCommand::Cell> strokeCells;
		strokeCells.reserve(cells.size());
		for (const glm::ivec2& coord : cells)
			strokeCells.push_back({ coord, tile });

		return std::make_unique<TileStrokeCommand>(layerIndex, std::move(strokeCells));
	}
}
