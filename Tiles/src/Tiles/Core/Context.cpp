#include "Context.h"

namespace Tiles
{
    std::shared_ptr<Context> Context::Create()
    {
        return std::make_shared<Context>();
    }

    Context::Context()
    {
		m_Project = std::make_shared<Project>(16, 16, "Untitled");

        // After any command runs, mark the project dirty and keep the working
        // layer in range (a layer delete can be undone/redone under us).
        m_CommandDispatcher.SetOnMutated([this]()
            {
                m_Project->MarkAsModified();
                ValidateWorkingLayer();
            });

        const auto& layerStack = m_Project->GetLayerStack();
        m_CameraController.Initialize(layerStack.GetWidth(), layerStack.GetHeight());

        m_Project->UpdateLastAccessed();
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
        if (m_Project->GetLayerStack().IsValidLayerIndex(index))
            m_EditingState.SetWorkingLayer(index);
    }

    bool Context::HasWorkingLayer() const
    {
        return m_Project->GetLayerStack().IsValidLayerIndex(m_EditingState.GetWorkingLayer());
    }

    TileLayer& Context::GetWorkingLayerRef()
    {
        return m_Project->GetLayerStack().GetLayer(m_EditingState.GetWorkingLayer());
    }

    const TileLayer& Context::GetWorkingLayerRef() const
    {
        return m_Project->GetLayerStack().GetLayer(m_EditingState.GetWorkingLayer());
    }

    void Context::PaintTile(size_t x, size_t y)
    {
        if (HasWorkingLayer())
            PaintTileOnLayer(m_EditingState.GetWorkingLayer(), x, y, m_EditingState.GetBrush());
    }

    void Context::PaintTileOnLayer(size_t layerIndex, size_t x, size_t y, const Tile& tile)
    {
        if (!m_Project->GetLayerStack().IsValidLayerIndex(layerIndex))
            return;

        ExecuteCommand(m_EditingState.BuildModeCommand(layerIndex, x, y, tile));
    }

    void Context::EraseTile(size_t x, size_t y)
    {
        if (HasWorkingLayer())
            ExecuteCommand(m_EditingState.BuildEraseCommand(m_EditingState.GetWorkingLayer(), x, y));
    }

    void Context::FillLayer(size_t x, size_t y)
    {
        if (HasWorkingLayer())
            ExecuteCommand(m_EditingState.BuildFillCommand(m_EditingState.GetWorkingLayer(), x, y, m_EditingState.GetBrush()));
    }

    void Context::ExecuteCommand(std::unique_ptr<Command> command)
    {
        if (m_Project)
            m_CommandDispatcher.Execute(std::move(command), m_Project->GetLayerStack());
    }

    void Context::Undo()
    {
        if (m_Project)
            m_CommandDispatcher.Undo(m_Project->GetLayerStack());
    }

    void Context::Redo()
    {
        if (m_Project)
            m_CommandDispatcher.Redo(m_Project->GetLayerStack());
    }

    void Context::ValidateWorkingLayer()
    {
        m_EditingState.ValidateWorkingLayer(m_Project->GetLayerStack());
    }

    void Context::CreateProject(const std::string& name, uint32_t width, uint32_t height)
    {
		m_CommandDispatcher.Clear();
        m_Project = std::make_shared<Project>(width, height, name);
        m_EditingState.Reset();

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

        m_CommandDispatcher.Clear();

        m_EditingState.Reset();
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