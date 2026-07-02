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

        const auto& layerStack = m_ProjectSession.GetProject()->GetLayerStack();
        m_CameraController.Initialize(layerStack.GetWidth(), layerStack.GetHeight());
    }

    void Context::ResetViewportCamera()
    {
        const auto& layerStack = m_ProjectSession.GetProject()->GetLayerStack();
        m_CameraController.Initialize(layerStack.GetWidth(), layerStack.GetHeight());
    }

    void Context::FitViewportCameraToProject()
    {
        const auto& layerStack = m_ProjectSession.GetProject()->GetLayerStack();
        m_CameraController.Fit(layerStack.GetWidth(), layerStack.GetHeight());
    }

    void Context::CenterViewportCameraOnProject()
    {
        const auto& layerStack = m_ProjectSession.GetProject()->GetLayerStack();
        m_CameraController.Center(layerStack.GetWidth(), layerStack.GetHeight());
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

    void Context::PaintTile(size_t x, size_t y)
    {
        if (HasWorkingLayer())
            PaintTileOnLayer(m_EditingState.GetWorkingLayer(), x, y, m_EditingState.GetBrush());
    }

    void Context::PaintTileOnLayer(size_t layerIndex, size_t x, size_t y, const Tile& tile)
    {
        if (!m_ProjectSession.GetProject()->GetLayerStack().IsValidLayerIndex(layerIndex))
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

    void Context::CreateProject(const std::string& name, uint32_t width, uint32_t height)
    {
        m_CommandDispatcher.Clear();
        m_ProjectSession.Create(name, width, height);
        m_EditingState.Reset();

        const auto& layerStack = m_ProjectSession.GetProject()->GetLayerStack();
        m_CameraController.Initialize(layerStack.GetWidth(), layerStack.GetHeight());
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

        const auto& layerStack = m_ProjectSession.GetProject()->GetLayerStack();
        m_CameraController.Initialize(layerStack.GetWidth(), layerStack.GetHeight());

        return result;
    }

    void Context::ResizeProject(uint32_t width, uint32_t height)
    {
        if (!m_ProjectSession.HasProject())
            return;

        const uint32_t oldWidth = m_ProjectSession.GetProject()->GetLayerStack().GetWidth();
        const uint32_t oldHeight = m_ProjectSession.GetProject()->GetLayerStack().GetHeight();

        m_ProjectSession.Resize(width, height);
        ValidateWorkingLayer();

        m_CameraController.FollowResize(oldWidth, oldHeight, width, height);
    }
}