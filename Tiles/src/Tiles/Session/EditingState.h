#pragma once

#include <cstddef>
#include <memory>

#include "Domain/Tile.h"

namespace Tiles
{
    class Command;
    class LayerStack;

    enum class PaintingMode
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
        EditingState() { Reset(); }

        size_t GetWorkingLayer() const { return m_WorkingLayer; }
        void SetWorkingLayer(size_t index) { m_WorkingLayer = index; }

        PaintingMode GetPaintingMode() const { return m_PaintingMode; }
        void SetPaintingMode(PaintingMode mode) { m_PaintingMode = mode; }

        const Tile& GetBrush() const { return m_Brush; }
        Tile& GetBrush() { return m_Brush; }
        void SetBrush(const Tile& brush) { m_Brush = brush; }

        // Restores the default selection: layer 0, no mode, a painted brush.
        void Reset();

        // Clamps the working layer back into range after the layer count changes
        // (e.g. a delete applied via undo/redo).
        void ValidateWorkingLayer(const LayerStack& layerStack);

        // Builds the command for the current painting mode; null for None.
        std::unique_ptr<Command> BuildModeCommand(size_t layerIndex, int x, int y, const Tile& tile) const;
        std::unique_ptr<Command> BuildEraseCommand(size_t layerIndex, int x, int y) const;
        std::unique_ptr<Command> BuildFillCommand(size_t layerIndex, int x, int y, const Tile& tile, const glm::ivec4& bounds) const;

    private:
        size_t m_WorkingLayer = 0;
        PaintingMode m_PaintingMode = PaintingMode::None;
        Tile m_Brush;
    };
}
