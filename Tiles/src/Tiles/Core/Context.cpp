#include "Context.h"

namespace Tiles
{
    std::shared_ptr<Context> Context::Create()
    {
        return std::make_shared<Context>();
    }

    Context::Context()
    {
        // After any command runs, mark the project dirty and keep the working
        // layer in range (a layer delete can be undone/redone under us).
        m_CommandDispatcher.SetOnMutated([this]()
            {
                m_ProjectSession.GetProject()->MarkAsModified();
                ValidateWorkingLayer();
            });

        m_CameraController.Initialize();
    }

    void Context::ResetViewportCamera()
    {
        m_CameraController.Initialize();
    }

    void Context::FitViewportCameraToProject()
    {
        const auto& layerStack = m_ProjectSession.GetProject()->GetLayerStack();
        m_CameraController.Fit(layerStack.GetBounds());
    }

    void Context::CenterViewportCameraOnProject()
    {
        m_CameraController.Center();
    }

    void Context::SetWorkingLayer(size_t index)
    {
        if (m_ProjectSession.GetProject()->GetLayerStack().IsValidLayerIndex(index))
            m_EditingState.SetWorkingLayer(index);
    }

    bool Context::HasWorkingLayer() const
    {
        return m_ProjectSession.GetProject()->GetLayerStack().IsValidLayerIndex(m_EditingState.GetWorkingLayer());
    }

    TileLayer& Context::GetWorkingLayerRef()
    {
        return m_ProjectSession.GetProject()->GetLayerStack().GetLayer(m_EditingState.GetWorkingLayer());
    }

    const TileLayer& Context::GetWorkingLayerRef() const
    {
        return m_ProjectSession.GetProject()->GetLayerStack().GetLayer(m_EditingState.GetWorkingLayer());
    }

    void Context::PaintTile(int x, int y)
    {
        if (HasWorkingLayer())
            PaintTileOnLayer(m_EditingState.GetWorkingLayer(), x, y, m_EditingState.GetBrush());
    }

    void Context::PaintTileOnLayer(size_t layerIndex, int x, int y, const Tile& tile)
    {
        if (!m_ProjectSession.GetProject()->GetLayerStack().IsValidLayerIndex(layerIndex))
            return;

        ExecuteCommand(m_EditingState.BuildModeCommand(layerIndex, x, y, tile));
    }

    void Context::EraseTile(int x, int y)
    {
        if (HasWorkingLayer())
            ExecuteCommand(m_EditingState.BuildEraseCommand(m_EditingState.GetWorkingLayer(), x, y));
    }

    void Context::FillLayer(int x, int y, const glm::ivec4& bounds)
    {
        if (HasWorkingLayer())
            ExecuteCommand(m_EditingState.BuildFillCommand(m_EditingState.GetWorkingLayer(), x, y, m_EditingState.GetBrush(), bounds));
    }

    void Context::ExecuteCommand(std::unique_ptr<Command> command)
    {
        if (m_ProjectSession.HasProject())
            m_CommandDispatcher.Execute(std::move(command), m_ProjectSession.GetProject()->GetLayerStack());
    }

    void Context::Undo()
    {
        if (m_ProjectSession.HasProject())
            m_CommandDispatcher.Undo(m_ProjectSession.GetProject()->GetLayerStack());
    }

    void Context::Redo()
    {
        if (m_ProjectSession.HasProject())
            m_CommandDispatcher.Redo(m_ProjectSession.GetProject()->GetLayerStack());
    }

    void Context::ValidateWorkingLayer()
    {
        m_EditingState.ValidateWorkingLayer(m_ProjectSession.GetProject()->GetLayerStack());
    }

    void Context::CreateProject(const std::string& name)
    {
        m_CommandDispatcher.Clear();
        m_ProjectSession.Create(name);
        m_EditingState.Reset();

        m_CameraController.Initialize();
    }

    ProjectResult Context::SaveProject()
    {
        return m_ProjectSession.Save();
    }

    ProjectResult Context::SaveProjectAs(const std::filesystem::path& path)
    {
        return m_ProjectSession.SaveAs(path);
    }

    ProjectResult Context::LoadProject(const std::filesystem::path& path)
    {
        ProjectResult result = m_ProjectSession.Load(path);
        if (!result.Success)
            return result;

        // A new project is in place; reset the editing/undo/camera state around it.
        m_CommandDispatcher.Clear();
        m_EditingState.Reset();
        ValidateWorkingLayer();

        m_CameraController.Initialize();

        return result;
    }
}