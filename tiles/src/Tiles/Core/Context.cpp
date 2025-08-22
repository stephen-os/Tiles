#include "Context.h"

#include "Commands/TilePaintCommand.h"
#include "Commands/TileEraseCommand.h"
#include "Commands/LayerFillCommand.h"

#include "Lumina/Core/Assert.h"

namespace Tiles
{
    Context::Context(std::shared_ptr<Project> project)
        : m_Project(project), m_WorkingLayer(0), m_PaintingMode(PaintingMode::None)
    {
        LUMINA_ASSERT(m_Project, "Project cannot be null");
        ValidateWorkingLayer();
        UpdateLastAccessed();

		m_Brush.SetPainted(true);
    }

    void Context::SetWorkingLayer(size_t index)
    {
        LayerStack& layerStack = m_Project->GetLayerStack();
        LUMINA_ASSERT(layerStack.IsValidLayerIndex(index), "Invalid working layer index");
        m_WorkingLayer = index;
    }

    bool Context::HasWorkingLayer() const
    {
        return m_Project->GetLayerStack().IsValidLayerIndex(m_WorkingLayer);
    }

    TileLayer& Context::GetWorkingLayerRef()
    {
        LUMINA_ASSERT(HasWorkingLayer(), "No valid working layer");
        return m_Project->GetLayerStack().GetLayer(m_WorkingLayer);
    }

    const TileLayer& Context::GetWorkingLayerRef() const
    {
        LUMINA_ASSERT(HasWorkingLayer(), "No valid working layer");
        return m_Project->GetLayerStack().GetLayer(m_WorkingLayer);
    }

    void Context::PaintTile(size_t x, size_t y)
    {
        LUMINA_ASSERT(HasWorkingLayer(), "No valid working layer for painting");
        PaintTileOnLayer(m_WorkingLayer, x, y, m_Brush);
    }

    void Context::PaintTileWithBrush(size_t x, size_t y, const Tile& brush)
    {
        LUMINA_ASSERT(HasWorkingLayer(), "No valid working layer for painting");
        PaintTileOnLayer(m_WorkingLayer, x, y, brush);
    }

    void Context::PaintTileOnLayer(size_t layerIndex, size_t x, size_t y, const Tile& tile)
    {
        LayerStack& layerStack = m_Project->GetLayerStack();
        LUMINA_ASSERT(layerStack.IsValidLayerIndex(layerIndex), "Invalid layer index for painting");

        switch (m_PaintingMode)
        {
        case PaintingMode::Brush:
        {
            auto command = std::make_unique<TilePaintCommand>(x, y, layerIndex, tile);
            ExecuteCommand(std::move(command));
            break;
        }
        case PaintingMode::Eraser:
        {
            auto command = std::make_unique<TileEraseCommand>(x, y, layerIndex);
            ExecuteCommand(std::move(command));
            break;
        }
        case PaintingMode::Fill:
        {
            auto command = std::make_unique<LayerFillCommand>(x, y, layerIndex, tile);
            ExecuteCommand(std::move(command));
            break;
        }
        }
    }

    void Context::EraseTile(size_t x, size_t y)
    {
        LUMINA_ASSERT(HasWorkingLayer(), "No valid working layer for erasing");
        auto command = std::make_unique<TileEraseCommand>(x, y, m_WorkingLayer);
        ExecuteCommand(std::move(command));
    }

    void Context::FillLayer(size_t x, size_t y)
    {
        LUMINA_ASSERT(HasWorkingLayer(), "No valid working layer for filling");
        auto command = std::make_unique<LayerFillCommand>(x, y, m_WorkingLayer, m_Brush);
        ExecuteCommand(std::move(command));
    }

    void Context::ExecuteCommand(std::unique_ptr<Command> command)
    {
        if (command)
        {
            m_CommandHistory.Execute(std::move(command), m_Project->GetLayerStack());
            m_Project->MarkAsModified();
        }
    }

    void Context::Undo()
    {
        if (CanUndo())
        {
            m_CommandHistory.Undo(m_Project->GetLayerStack());
            m_Project->MarkAsModified();
        }
    }

    void Context::Redo()
    {
        if (CanRedo())
        {
            m_CommandHistory.Redo(m_Project->GetLayerStack());
            m_Project->MarkAsModified();
        }
    }

    void Context::ValidateWorkingLayer()
    {
        LayerStack& layerStack = m_Project->GetLayerStack();
        if (!HasWorkingLayer() && !layerStack.IsEmpty())
        {
            m_WorkingLayer = 0;
        }
        else if (layerStack.IsEmpty())
        {
            m_WorkingLayer = 0;
        }
        else if (m_WorkingLayer >= layerStack.GetLayerCount())
        {
            m_WorkingLayer = layerStack.GetLayerCount() - 1;
        }
    }
}