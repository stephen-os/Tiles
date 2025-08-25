#pragma once

#include <filesystem>

#include "Tile.h"
#include "Project.h"
#include "CommandHistory.h"

#include "Constants.h"

#include "Lumina/Lumina.h"

#include "Base.h"

namespace Tiles
{
    enum class PaintingMode
    {
        None = 0,
        Brush,
        Eraser,
        Fill
    };

    struct ProjectResult
    {
		bool Success = false;
		std::string Message;
    };

    class Context
    {
    public:   
        static Ref<Context> Create();

        Context();
        ~Context() = default;

        Ref<Lumina::OrthographicCamera> GetViewportCamera() { return m_ViewportCamera; }
        const Ref<Lumina::OrthographicCamera> GetViewportCamera() const { return m_ViewportCamera; }
        void ResetViewportCamera();
        void FitViewportCameraToProject();
        void CenterViewportCameraOnProject();

        void SetWorkingLayer(size_t index);
        size_t GetWorkingLayer() const { return m_WorkingLayer; }
        bool HasWorkingLayer() const;
        TileLayer& GetWorkingLayerRef();
        const TileLayer& GetWorkingLayerRef() const;

        void SetPaintingMode(PaintingMode mode) { m_PaintingMode = mode; }
        PaintingMode GetPaintingMode() const { return m_PaintingMode; }
        void SetBrush(const Tile& brush) { m_Brush = brush; }
        const Tile& GetBrush() const { return m_Brush; }
        Tile& GetBrush() { return m_Brush; }

        void PaintTile(size_t x, size_t y);
        void PaintTileWithBrush(size_t x, size_t y, const Tile& brush);
        void PaintTileOnLayer(size_t layerIndex, size_t x, size_t y, const Tile& tile);
        void EraseTile(size_t x, size_t y);
        void FillLayer(size_t x, size_t y);

        void ExecuteCommand(std::unique_ptr<Command> command);
        bool CanUndo() const { return m_CommandHistory.CanUndo(); }
        bool CanRedo() const { return m_CommandHistory.CanRedo(); }
        void Undo();
        void Redo();

        bool IsDirty() const { return m_Project->HasUnsavedChanges(); }

		void ClearHistory() { m_CommandHistory.Clear(); }   

		// Project Management
        void CreateProject(const std::string& name, uint32_t width, uint32_t height);
        ProjectResult SaveProject();
        ProjectResult SaveProjectAs(const std::filesystem::path& path);
        ProjectResult LoadProject(const std::filesystem::path& path);
        void ResizeProject(uint32_t width, uint32_t height);
        bool HasProject() const { return m_Project != nullptr; }
        std::string GetProjectDisplayName() const;
        Ref<Project> GetProject() { return m_Project; }
        const Ref<Project> GetProject() const { return m_Project; }


    private:
        void ValidateWorkingLayer();
        void InitializeSceneCamera();

    private: 
        CommandHistory m_CommandHistory;

        Ref<Project> m_Project;
        
        Ref<Lumina::OrthographicCamera> m_ViewportCamera;
        
        size_t m_WorkingLayer = 0;
        PaintingMode m_PaintingMode = PaintingMode::None;
        Tile m_Brush;
    };
}