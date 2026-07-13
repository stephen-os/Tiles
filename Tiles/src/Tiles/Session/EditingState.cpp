#include "EditingState.h"

#include "Domain/LayerStack.h"
#include "Commands/TilePaintCommand.h"
#include "Commands/TileEraseCommand.h"
#include "Commands/LayerFillCommand.h"

namespace Tiles
{
    void EditingState::Reset()
    {
        m_WorkingLayer = 0;
        m_PaintingMode = PaintingMode::None;
        m_Brush = Tile();
        m_Brush.SetPainted(true);
    }

    void EditingState::ValidateWorkingLayer(const LayerStack& layerStack)
    {
        if (layerStack.IsEmpty())
        {
            m_WorkingLayer = 0;
        }
        else if (!layerStack.IsValidLayerIndex(m_WorkingLayer))
        {
            m_WorkingLayer = 0;
        }
        else if (m_WorkingLayer >= layerStack.GetLayerCount())
        {
            m_WorkingLayer = layerStack.GetLayerCount() - 1;
        }
    }

    std::unique_ptr<Command> EditingState::BuildModeCommand(size_t layerIndex, int x, int y, const Tile& tile) const
    {
        switch (m_PaintingMode)
        {
        case PaintingMode::Brush:
            return std::make_unique<TilePaintCommand>(x, y, layerIndex, tile);
        case PaintingMode::Eraser:
            return std::make_unique<TileEraseCommand>(x, y, layerIndex);
        // Fill needs the visible-view bound, so it goes through Context::FillLayer
        // / BuildFillCommand rather than this per-cell dispatch.
        default:
            return nullptr;
        }
    }

    std::unique_ptr<Command> EditingState::BuildEraseCommand(size_t layerIndex, int x, int y) const
    {
        return std::make_unique<TileEraseCommand>(x, y, layerIndex);
    }

    std::unique_ptr<Command> EditingState::BuildFillCommand(size_t layerIndex, int x, int y, const Tile& tile, const glm::ivec4& bounds) const
    {
        return std::make_unique<LayerFillCommand>(x, y, layerIndex, tile, bounds);
    }
}
