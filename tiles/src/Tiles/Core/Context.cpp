#include "Context.h"

#include "Commands/TilePaintCommand.h"
#include "Commands/TileEraseCommand.h"
#include "Commands/LayerFillCommand.h"

#include <algorithm>

#include "json.hpp"

namespace Tiles
{
    Lumina::Ref<Context> Context::Create()
    {
        return CreateRef<Context>();
    }

    Context::Context()
    {
		m_Project = CreateRef<Project>(16, 16, "Untitled");

        m_ViewportCamera = Lumina::CreateRef<Lumina::OrthographicCamera>();
        InitializeSceneCamera();
      
        m_Project->UpdateLastAccessed();

        m_Brush.SetPainted(true);
    }

    void Context::InitializeSceneCamera()
    {
        const auto& layerStack = m_Project->GetLayerStack();
        const float gridWidth = static_cast<float>(layerStack.GetWidth());
        const float gridHeight = static_cast<float>(layerStack.GetHeight());

        m_ViewportCamera->SetPosition({
            Viewport::Render::DefaultTileSize * (gridWidth * 0.5f),
            Viewport::Render::DefaultTileSize * (gridHeight * 0.5f),
            1.0f
            });

        m_ViewportCamera->SetZoom(1.0f);
    }

    void Context::ResetViewportCamera()
    {
        InitializeSceneCamera();
    }

    void Context::FitViewportCameraToProject()
    {
        const auto& layerStack = m_Project->GetLayerStack();
        const float projectWidth = layerStack.GetWidth() * Viewport::Render::DefaultTileSize;
        const float projectHeight = layerStack.GetHeight() * Viewport::Render::DefaultTileSize;

        CenterViewportCameraOnProject();

        const float viewportWidth = 800.0f;
        const float viewportHeight = 600.0f;

        const float zoomX = viewportWidth / projectWidth;
        const float zoomY = viewportHeight / projectHeight;
        const float fitZoom = std::min(zoomX, zoomY) * 0.9f;

        const float clampedZoom = std::clamp(fitZoom, Viewport::Render::MinZoom, Viewport::Render::MaxZoom);
        m_ViewportCamera->SetZoom(clampedZoom);
    }

    void Context::CenterViewportCameraOnProject()
    {
        const auto& layerStack = m_Project->GetLayerStack();
        const float gridWidth = static_cast<float>(layerStack.GetWidth());
        const float gridHeight = static_cast<float>(layerStack.GetHeight());

        glm::vec3 currentPos = m_ViewportCamera->GetPosition();
        m_ViewportCamera->SetPosition({
            Viewport::Render::DefaultTileSize * (gridWidth * 0.5f),
            Viewport::Render::DefaultTileSize * (gridHeight * 0.5f),
            currentPos.z
            });
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

    void Context::PaintTileWithBrush(size_t x, size_t y, const Tile& brush)
    {
        if (HasWorkingLayer())
        {
            PaintTileOnLayer(m_WorkingLayer, x, y, brush);
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

    void Context::CreateProject(const std::string& name, uint32_t width, uint32_t height)
    {
		m_CommandHistory.Clear();
        m_Project = Lumina::CreateRef<Project>(width, height, name);
        m_WorkingLayer = 0;
        m_PaintingMode = PaintingMode::None;
        m_Brush = Tile();
        m_Brush.SetPainted(true);
        InitializeSceneCamera();
        m_Project->UpdateLastAccessed();
    }

    ProjectResult Context::SaveProject()
    {
        if (!m_Project)
            return { false, "No project loaded." };

        if (m_Project->IsNew())
            return { false, "Project has no file path. Use 'Save As' to specify a location." };

        auto path = m_Project->GetFilePath();

        auto directory = path.parent_path();
        if (!directory.empty() && !std::filesystem::exists(directory))
        {
            std::filesystem::create_directories(directory);
        }

        try
        {
            nlohmann::json projectJSON = m_Project->ToJSON();

            std::ofstream file(path);

            if (!file.is_open())
            {
                return { false, "Failed to open file for writing." };
            }

            file << projectJSON.dump(4);
            file.close();

            m_Project->MarkAsSaved();
            m_Project->UpdateLastAccessed();

            return { true, "Project saved successfully." };
        }
        catch (const std::exception& e)
        {
            return { false, std::string("Failed to save project: ") + e.what() };
        }
    }

    ProjectResult Context::SaveProjectAs(const std::filesystem::path& path)
    {
        if (!m_Project)
            return { false, "No project loaded." };

        if (path.empty())
            return { false, "Invalid file path." };

        auto directory = path.parent_path();
        if (!directory.empty() && !std::filesystem::exists(directory))
        {
            std::filesystem::create_directories(directory);
        }

        try
        {
            nlohmann::json projectJSON = m_Project->ToJSON();

            std::ofstream file(path);

            if (!file.is_open())
            {
                return { false, "Failed to open file for writing." };
            }

            file << projectJSON.dump(4);
            file.close();

            m_Project->SetFilePath(path.string());
            m_Project->MarkAsSaved();
            m_Project->UpdateLastAccessed();

            return { true, "Project saved successfully." };
        }
        catch (const std::exception& e)
        {
            return { false, std::string("Failed to save project: ") + e.what() };
        }
    }

    ProjectResult Context::LoadProject(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
        {
            return { false, "File does not exist." };
        }


        Ref<Project> project;

        try
        {
			std::ifstream file(path);
            if (!file.is_open())
            {
                return { false, "Failed to open file for reading." };
            }

            nlohmann::json jsonProject;
            file >> jsonProject;
            file.close();
            
            project = Project::FromJSON(jsonProject);
            
            project->SetFilePath(path.string());
            project->MarkAsSaved();
            project->UpdateLastAccessed();
        }
        catch (const std::exception& e)
        {
            return { false, std::string("Failed to load project: ") + e.what() };
        }

		m_Project = project;

        m_CommandHistory.Clear();

        m_WorkingLayer = 0;
        m_PaintingMode = PaintingMode::None;
        m_Brush = Tile();
        m_Brush.SetPainted(true);
        InitializeSceneCamera();
        return { true, "Project loaded successfully." };
    }

    void Context::ResizeProject(uint32_t width, uint32_t height)
    {
        if (!m_Project)
            return;

        const auto& layerStack = m_Project->GetLayerStack();
        const float oldWidth = static_cast<float>(layerStack.GetWidth());
        const float oldHeight = static_cast<float>(layerStack.GetHeight());

        m_Project->GetLayerStack().Resize(width, height);
        m_Project->MarkAsModified();
        ValidateWorkingLayer();

        glm::vec3 currentPos = m_ViewportCamera->GetPosition();
        const float relativeX = currentPos.x / (oldWidth * Viewport::Render::DefaultTileSize);
        const float relativeY = currentPos.y / (oldHeight * Viewport::Render::DefaultTileSize);

        const float clampedX = std::clamp(relativeX, 0.0f, 1.0f);
        const float clampedY = std::clamp(relativeY, 0.0f, 1.0f);

        m_ViewportCamera->SetPosition({
            clampedX * width * Viewport::Render::DefaultTileSize,
            clampedY * height * Viewport::Render::DefaultTileSize,
            currentPos.z
            });

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