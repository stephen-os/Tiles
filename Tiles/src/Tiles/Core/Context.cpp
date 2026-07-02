#include "Context.h"

#include "Commands/TilePaintCommand.h"
#include "Commands/TileEraseCommand.h"
#include "Commands/LayerFillCommand.h"

namespace Tiles
{
    std::shared_ptr<Context> Context::Create()
    {
        return std::make_shared<Context>();
    }

    Context::Context()
    {
		m_Project = std::make_shared<Project>(16, 16, "Untitled");

        const auto& layerStack = m_Project->GetLayerStack();
        m_CameraController.Initialize(layerStack.GetWidth(), layerStack.GetHeight());

        m_Project->UpdateLastAccessed();

        m_Brush.SetPainted(true);
    }

    void Context::ResetViewportCamera()
    {
        const auto& layerStack = m_Project->GetLayerStack();
        m_CameraController.Initialize(layerStack.GetWidth(), layerStack.GetHeight());
    }

    void Context::FitViewportCameraToProject()
    {
        const auto& layerStack = m_Project->GetLayerStack();
        m_CameraController.Fit(layerStack.GetWidth(), layerStack.GetHeight());
    }

    void Context::CenterViewportCameraOnProject()
    {
        const auto& layerStack = m_Project->GetLayerStack();
        m_CameraController.Center(layerStack.GetWidth(), layerStack.GetHeight());
    }

    void Context::SetWorkingLayer(size_t index)
    {
        LayerStack& layerStack = m_Project->GetLayerStack();
        if (layerStack.IsValidLayerIndex(index))
        {
            m_WorkingLayer = index;
        }
    }

    bool Context::HasWorkingLayer() const
    {
        return m_Project->GetLayerStack().IsValidLayerIndex(m_WorkingLayer);
    }

    TileLayer& Context::GetWorkingLayerRef()
    {
        return m_Project->GetLayerStack().GetLayer(m_WorkingLayer);
    }

    const TileLayer& Context::GetWorkingLayerRef() const
    {
        return m_Project->GetLayerStack().GetLayer(m_WorkingLayer);
    }

    void Context::PaintTile(size_t x, size_t y)
    {
        if (HasWorkingLayer())
        {
            PaintTileOnLayer(m_WorkingLayer, x, y, m_Brush);
        }
    }

    void Context::PaintTileOnLayer(size_t layerIndex, size_t x, size_t y, const Tile& tile)
    {
        LayerStack& layerStack = m_Project->GetLayerStack();
        if (!layerStack.IsValidLayerIndex(layerIndex))
            return;

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
        if (HasWorkingLayer())
        {
            auto command = std::make_unique<TileEraseCommand>(x, y, m_WorkingLayer);
            ExecuteCommand(std::move(command));
        }
    }

    void Context::FillLayer(size_t x, size_t y)
    {
        if (HasWorkingLayer())
        {
            auto command = std::make_unique<LayerFillCommand>(x, y, m_WorkingLayer, m_Brush);
            ExecuteCommand(std::move(command));
        }
    }

    void Context::ExecuteCommand(std::unique_ptr<Command> command)
    {
        if (command && m_Project)
        {
            m_CommandHistory.Execute(std::move(command), m_Project->GetLayerStack());
            m_Project->MarkAsModified();
            ValidateWorkingLayer();
        }
    }

    void Context::Undo()
    {
        if (CanUndo())
        {
            m_CommandHistory.Undo(m_Project->GetLayerStack());
            m_Project->MarkAsModified();
            ValidateWorkingLayer();
        }
    }

    void Context::Redo()
    {
        if (CanRedo())
        {
            m_CommandHistory.Redo(m_Project->GetLayerStack());
            m_Project->MarkAsModified();
            ValidateWorkingLayer();
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

    void Context::CreateProject(const std::string& name, uint32_t width, uint32_t height)
    {
		m_CommandHistory.Clear();
        m_Project = std::make_shared<Project>(width, height, name);
        m_WorkingLayer = 0;
        m_PaintingMode = PaintingMode::None;
        m_Brush = Tile();
        m_Brush.SetPainted(true);

        const auto& layerStack = m_Project->GetLayerStack();
        m_CameraController.Initialize(layerStack.GetWidth(), layerStack.GetHeight());

        m_Project->UpdateLastAccessed();
    }

    ProjectResult Context::SaveProject()
    {
        if (!m_Project)
            return { false, "No project loaded." };

        if (m_Project->IsNew())
            return { false, "Project has no file path. Use 'Save As' to specify a location." };

        const auto path = m_Project->GetFilePath();

        ProjectResult result = ProjectSerializer::Save(*m_Project, path);
        if (!result.Success)
            return result;

        m_Project->MarkAsSaved();
        m_Project->UpdateLastAccessed();
        m_ProjectHistory.AddProject(path, m_Project->GetProjectName());

        TILES_LOG_INFO("Context::SaveProject: Successfully saved project '{}'", m_Project->GetProjectName());
        return result;
    }

    ProjectResult Context::SaveProjectAs(const std::filesystem::path& path)
    {
        if (!m_Project)
            return { false, "No project loaded." };

        if (path.empty())
            return { false, "Invalid file path." };

        ProjectResult result = ProjectSerializer::Save(*m_Project, path);
        if (!result.Success)
            return result;

        m_Project->SetFilePath(path.string());
        m_Project->MarkAsSaved();
        m_Project->UpdateLastAccessed();
        m_ProjectHistory.AddProject(path, m_Project->GetProjectName());

        TILES_LOG_INFO("Context::SaveProjectAs: Successfully saved project '{}' to '{}'", m_Project->GetProjectName(), path.string());
        return result;
    }

    ProjectResult Context::LoadProject(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
        {
            TILES_LOG_INFO("Context::LoadProject: File does not exist: {}", path.string());
            m_ProjectHistory.RemoveProject(path);
            return { false, "File does not exist." };
        }

        std::shared_ptr<Project> project;
        ProjectResult result = ProjectSerializer::Load(path, project);
        if (!result.Success)
        {
            TILES_LOG_INFO("Context::LoadProject: Load failed: {}", result.Message);
            return result;
        }

        project->SetFilePath(path.string());
        project->MarkAsSaved();
        project->UpdateLastAccessed();

        m_Project = project;

        m_CommandHistory.Clear();

        m_WorkingLayer = 0;
        m_PaintingMode = PaintingMode::None;
        m_Brush = Tile();
        m_Brush.SetPainted(true);

        ValidateWorkingLayer();

        const auto& layerStack = m_Project->GetLayerStack();
        m_CameraController.Initialize(layerStack.GetWidth(), layerStack.GetHeight());

        m_ProjectHistory.AddProject(path, m_Project->GetProjectName());

        TILES_LOG_INFO("Context::LoadProject: Successfully loaded project '{}' from '{}'", m_Project->GetProjectName(), path.string());
        return result;
    }

    void Context::ResizeProject(uint32_t width, uint32_t height)
    {
        if (!m_Project)
            return;

        const uint32_t oldWidth = m_Project->GetLayerStack().GetWidth();
        const uint32_t oldHeight = m_Project->GetLayerStack().GetHeight();

        m_Project->GetLayerStack().Resize(width, height);
        m_Project->MarkAsModified();
        ValidateWorkingLayer();

        m_CameraController.FollowResize(oldWidth, oldHeight, width, height);

		m_Project->UpdateLastAccessed();
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