#include "EditingState.h"

#include "Domain/LayerStack.h"
#include "Commands/LayerFillCommand.h"
#include "Commands/TileStrokeCommand.h"

#include <algorithm>
#include <cstdlib>

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
		// Brush and shape tools stamp the brush tile; eraser stamps an empty tile
		// (which clears the cell). None/Fill do not stroke.
		Tile tile;
		if (m_PaintingMode == PaintingMode::Brush || IsShapeMode(m_PaintingMode))
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

	// Bresenham line cells from a to b, inclusive.
	static std::vector<glm::ivec2> LineCells(glm::ivec2 a, glm::ivec2 b)
	{
		std::vector<glm::ivec2> cells;

		const int sx = a.x < b.x ? 1 : -1;
		const int sy = a.y < b.y ? 1 : -1;
		const int dx = std::abs(b.x - a.x);
		const int dy = -std::abs(b.y - a.y);
		int err = dx + dy;

		glm::ivec2 p = a;
		while (true)
		{
			cells.push_back(p);
			if (p == b)
				break;

			const int e2 = 2 * err;
			if (e2 >= dy) { err += dy; p.x += sx; }
			if (e2 <= dx) { err += dx; p.y += sy; }
		}

		return cells;
	}

	// Rectangle cells spanning a..b; the four edges (outline) or solid.
	static std::vector<glm::ivec2> RectangleCells(glm::ivec2 a, glm::ivec2 b, bool filled)
	{
		const int x0 = std::min(a.x, b.x), x1 = std::max(a.x, b.x);
		const int y0 = std::min(a.y, b.y), y1 = std::max(a.y, b.y);

		std::vector<glm::ivec2> cells;

		if (filled)
		{
			for (int y = y0; y <= y1; ++y)
				for (int x = x0; x <= x1; ++x)
					cells.push_back({ x, y });
		}
		else
		{
			for (int x = x0; x <= x1; ++x)
			{
				cells.push_back({ x, y0 });
				cells.push_back({ x, y1 });
			}
			for (int y = y0 + 1; y < y1; ++y)
			{
				cells.push_back({ x0, y });
				cells.push_back({ x1, y });
			}
		}

		return cells;
	}

	// Ellipse cells inscribed in the a..b box; boundary (outline) or solid. A cell
	// is inside when ((x-cx)/rx)^2 + ((y-cy)/ry)^2 <= 1; the outline keeps inside
	// cells that have an outside 4-neighbour.
	static std::vector<glm::ivec2> EllipseCells(glm::ivec2 a, glm::ivec2 b, bool filled)
	{
		const int x0 = std::min(a.x, b.x), x1 = std::max(a.x, b.x);
		const int y0 = std::min(a.y, b.y), y1 = std::max(a.y, b.y);

		const float cx = (x0 + x1) * 0.5f;
		const float cy = (y0 + y1) * 0.5f;
		const float rx = std::max((x1 - x0) * 0.5f, 0.5f);
		const float ry = std::max((y1 - y0) * 0.5f, 0.5f);

		const auto inside = [&](int x, int y)
		{
			const float nx = (x - cx) / rx;
			const float ny = (y - cy) / ry;
			return nx * nx + ny * ny <= 1.0f;
		};

		std::vector<glm::ivec2> cells;
		for (int y = y0; y <= y1; ++y)
			for (int x = x0; x <= x1; ++x)
			{
				if (!inside(x, y))
					continue;
				if (filled || !inside(x - 1, y) || !inside(x + 1, y) || !inside(x, y - 1) || !inside(x, y + 1))
					cells.push_back({ x, y });
			}

		return cells;
	}

	// The cells of the current shape tool between start and end; see header.
	std::vector<glm::ivec2> EditingState::ShapeCells(const glm::ivec2& start, const glm::ivec2& end) const
	{
		switch (m_PaintingMode)
		{
		case PaintingMode::Line:      return LineCells(start, end);
		case PaintingMode::Rectangle: return RectangleCells(start, end, m_ShapeFilled);
		case PaintingMode::Ellipse:   return EllipseCells(start, end, m_ShapeFilled);
		default:                      return {};
		}
	}
}
