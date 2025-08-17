#pragma once

#include <memory>

#include "Tile.h"
#include "Project.h"

#include "CommandHistory.h"

namespace Tiles
{
    enum class PaintingMode
    {
        None = 0,   
        Brush,
        Eraser,
        Fill
    };

    class Context
    {
    public:
        Context(std::shared_ptr<Project> project);
        ~Context() = default;

        // Project access
        Project& GetProject() { return *m_Project; }
        const Project& GetProject() const { return *m_Project; }
        std::shared_ptr<Project> GetProjectPtr() { return m_Project; }

        // Working layer management
        void SetWorkingLayer(size_t index);
        size_t GetWorkingLayer() const { return m_WorkingLayer; }
        bool HasWorkingLayer() const;
        TileLayer& GetWorkingLayerRef();
        const TileLayer& GetWorkingLayerRef() const;

        // Painting state
        void SetPaintingMode(PaintingMode mode) { m_PaintingMode = mode; }
        PaintingMode GetPaintingMode() const { return m_PaintingMode; }

        void SetBrush(const Tile& brush) { m_Brush = brush; }
        const Tile& GetBrush() const { return m_Brush; }
        Tile& GetBrush() { return m_Brush; }

        // Painting operations
        void PaintTile(size_t x, size_t y);
        void PaintTileWithBrush(size_t x, size_t y, const Tile& brush);
        void PaintTileOnLayer(size_t layerIndex, size_t x, size_t y, const Tile& tile);
        void EraseTile(size_t x, size_t y);
        void FillLayer(size_t x, size_t y);

        // Command system
        void ExecuteCommand(std::unique_ptr<Command> command);
        bool CanUndo() const { return m_CommandHistory.CanUndo(); }
        bool CanRedo() const { return m_CommandHistory.CanRedo(); }
        void Undo();
        void Redo();
        void ClearHistory() { m_CommandHistory.Clear(); }

        // Context state
        bool IsDirty() const { return m_Project->HasUnsavedChanges(); }
        void UpdateLastAccessed() { m_Project->UpdateLastAccessed(); }

    private:
        void ValidateWorkingLayer();

        std::shared_ptr<Project> m_Project;
        CommandHistory m_CommandHistory;

        size_t m_WorkingLayer = 0;
        PaintingMode m_PaintingMode = PaintingMode::None;
        Tile m_Brush;
    };
}