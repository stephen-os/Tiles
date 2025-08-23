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

    // Add these implementations to your Context.cpp file:

    void Context::NewProject(const std::string& name, uint32_t width, uint32_t height)
    {
        // Clear command history for new project
        ClearHistory();

        // Create new project
        m_Project = CreateRef<Project>(width, height, name);

        // Reset working layer to 0 (the default layer)
        m_WorkingLayer = 0;

        // Reset painting state
        m_PaintingMode = PaintingMode::None;
        m_Brush = Tile(); // Reset to default tile

        // Update access time
        UpdateLastAccessed();
    }

    bool Context::LoadProject(const std::filesystem::path& filePath)
    {
        try
        {
            // Clear command history before loading
            ClearHistory();

            // Load project from file
            auto loadedProject = Project::Deserialize(filePath);
            if (!loadedProject)
            {
                return false;
            }

            // Replace current project
            m_Project = std::shared_ptr<Project>(std::move(loadedProject));

            // Reset context state for loaded project
            m_WorkingLayer = 0;
            m_PaintingMode = PaintingMode::None;
            m_Brush = Tile();

            // Validate working layer
            ValidateWorkingLayer();

            // Update access time
            UpdateLastAccessed();

            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    bool Context::SaveProject()
    {
        if (!m_Project)
            return false;

        // If project is new (no file path), we need to save as
        if (m_Project->IsNew())
        {
            // You might want to trigger a file dialog here
            // For now, return false to indicate "Save As" is needed
            return false;
        }

        try
        {
            Project::Serialize(*m_Project, m_Project->GetFilePath());
            m_Project->MarkAsSaved();
            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    bool Context::SaveProjectAs(const std::filesystem::path& filePath)
    {
        if (!m_Project)
            return false;

        try
        {
            // Update project file path
            m_Project->SetFilePath(filePath.string());

            // Save to the new location
            Project::Serialize(*m_Project, filePath);
            m_Project->MarkAsSaved();

            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    void Context::ResizeProject(uint32_t width, uint32_t height)
    {
        if (!m_Project)
            return;

        // Resize the layer stack
        m_Project->GetLayerStack().Resize(width, height);

        // Mark project as modified
        m_Project->MarkAsModified();

        // Validate working layer after resize
        ValidateWorkingLayer();

        // Update access time
        UpdateLastAccessed();
    }

    std::string Context::GetProjectDisplayName() const
    {
        if (!m_Project)
            return "No Project";

        std::string name = m_Project->GetProjectName();

        if (m_Project->IsNew())
        {
            name += " (Unsaved)";
        }

        return name;
    }
}