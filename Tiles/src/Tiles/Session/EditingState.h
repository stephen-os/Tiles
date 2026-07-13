#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

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
		Fill
	};

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

		// Restores the default selection: layer 0, no mode, a painted brush.
		void Reset();

		// Clamps the working layer back into range after the layer count changes
		// (e.g. a delete applied via undo/redo).
		void ValidateWorkingLayer(const LayerStack& layerStack);

		// Builds the command for the current painting mode; null for None.
		[[nodiscard]] std::unique_ptr<Command> BuildModeCommand(size_t layerIndex, int x, int y, const Tile& tile) const;

		// Builds an erase command for the cell at (x, y).
		[[nodiscard]] std::unique_ptr<Command> BuildEraseCommand(size_t layerIndex, int x, int y) const;

		// Builds a flood-fill command bounded by the visible region.
		[[nodiscard]] std::unique_ptr<Command> BuildFillCommand(size_t layerIndex, int x, int y, const Tile& tile, const glm::ivec4& bounds) const;

	private:
		size_t m_WorkingLayer = 0;
		PaintingMode m_PaintingMode = PaintingMode::None;
		Tile m_Brush;
	};
}
