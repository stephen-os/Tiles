#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Domain/Tile.h"

namespace Tiles
{
	class Command;
	class LayerStack;

	// The tool applied to the working layer when painting.
	enum class PaintingMode : uint8_t
	{
		None = 0,
		Brush,
		Eraser,
		Fill,
		Line,
		Rectangle,
		Ellipse,
		Select
	};

	// True for the drag-out shape tools (Line / Rectangle / Ellipse).
	[[nodiscard]] constexpr bool IsShapeMode(PaintingMode mode)
	{
		return mode == PaintingMode::Line || mode == PaintingMode::Rectangle || mode == PaintingMode::Ellipse;
	}

	// Holds the transient editing selection - active layer, brush, and painting
	// mode - and builds the edit commands they imply. It owns no project state;
	// the one layer-count-dependent operation (working-layer validation) takes
	// the stack as a parameter.
	class EditingState
	{
	public:
		// Starts at the default selection (see Reset).
		EditingState() { Reset(); }

		// The index of the layer edits target.
		[[nodiscard]] size_t GetWorkingLayer() const { return m_WorkingLayer; }

		// Selects the layer edits target.
		void SetWorkingLayer(size_t index) { m_WorkingLayer = index; }

		// The active painting tool.
		[[nodiscard]] PaintingMode GetPaintingMode() const { return m_PaintingMode; }

		// Sets the active painting tool.
		void SetPaintingMode(PaintingMode mode) { m_PaintingMode = mode; }

		// The tile stamped by the brush and fill tools.
		[[nodiscard]] const Tile& GetBrush() const { return m_Brush; }
		[[nodiscard]] Tile& GetBrush() { return m_Brush; }

		// Sets the tile stamped by the brush and fill tools.
		void SetBrush(const Tile& brush) { m_Brush = brush; }

		// The brush footprint size (N x N cells); always at least 1.
		[[nodiscard]] int GetBrushSize() const { return m_BrushSize; }

		// Sets the brush footprint size, clamped to at least 1.
		void SetBrushSize(int size) { m_BrushSize = size < 1 ? 1 : size; }

		// A row-major grid of tiles (GetStampSize().x by .y) the brush tool places
		// as one centered block; empty means the ordinary single-tile brush.
		[[nodiscard]] bool HasStamp() const { return !m_StampTiles.empty(); }
		[[nodiscard]] const std::vector<Tile>& GetStampTiles() const { return m_StampTiles; }
		[[nodiscard]] glm::ivec2 GetStampSize() const { return m_StampSize; }

		// Sets the stamp grid (row-major, width * height tiles); clears it if the
		// dimensions are < 1 or the tile count does not match.
		void SetStamp(std::vector<Tile> tiles, int width, int height);
		void ClearStamp() { m_StampTiles.clear(); m_StampSize = { 0, 0 }; }

		// Restores the default selection: layer 0, no mode, a painted brush.
		void Reset();

		// Clamps the working layer back into range after the layer count changes
		// (e.g. a delete applied via undo/redo).
		void ValidateWorkingLayer(const LayerStack& layerStack);

		// Builds a flood-fill command bounded by the visible region.
		[[nodiscard]] std::unique_ptr<Command> BuildFillCommand(size_t layerIndex, int x, int y, const Tile& tile, const glm::ivec4& bounds) const;

		// The cells the brush covers when centered on (cx, cy): an N x N block
		// for brush size N.
		[[nodiscard]] std::vector<glm::ivec2> BrushFootprint(int cx, int cy) const;

		// Builds a stroke command that applies the brush (or an erase) to every
		// cell as one undo step; null for None/Fill or an empty cell set.
		[[nodiscard]] std::unique_ptr<Command> BuildStrokeCommand(size_t layerIndex, const std::vector<glm::ivec2>& cells) const;

		// Builds a command that places the stamp centered on `anchor` as one undo
		// step, each cell carrying its own atlas tile; null if there is no stamp.
		[[nodiscard]] std::unique_ptr<Command> BuildStampCommand(size_t layerIndex, const glm::ivec2& anchor) const;

		// Builds a command that moves `cells` (all on layerIndex) by `offset` as one
		// undo step: each source cell is cleared and its tile placed at the shifted
		// destination (destinations win where they overlap sources). Reads the source
		// tiles from `layerStack`; null for an empty selection or a zero offset.
		[[nodiscard]] std::unique_ptr<Command> BuildMoveCommand(size_t layerIndex, const std::vector<glm::ivec2>& cells, const glm::ivec2& offset, const LayerStack& layerStack) const;

		// Whether shape tools (rectangle / ellipse) paint solid or outline.
		[[nodiscard]] bool GetShapeFilled() const { return m_ShapeFilled; }
		void SetShapeFilled(bool filled) { m_ShapeFilled = filled; }

		// The cells of the current shape tool spanning start..end (line, rectangle,
		// or ellipse); empty for a non-shape mode.
		[[nodiscard]] std::vector<glm::ivec2> ShapeCells(const glm::ivec2& start, const glm::ivec2& end) const;

	private:
		size_t m_WorkingLayer = 0;
		PaintingMode m_PaintingMode = PaintingMode::None;
		Tile m_Brush;
		int m_BrushSize = 1;
		bool m_ShapeFilled = false;
		std::vector<Tile> m_StampTiles;         // row-major; empty => single-tile brush
		glm::ivec2 m_StampSize = { 0, 0 };      // stamp width x height in cells
	};
}
