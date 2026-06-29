#pragma once

#include "../../Domain/Entities/TileProject.h"
#include "../../Domain/ValueObjects/TileData.h"
#include "../../Domain/ValueObjects/Position.h"
#include "CommandHistory.h"

#include <memory>
#include <optional>

namespace Tiles::Services
{
    enum class PaintingTool
    {
        None,
        Brush,
        Eraser,
        Fill
    };

    // Application service for painting operations
    // Manages the current brush, tool, and painting commands
    class PaintingService
    {
    public:
        explicit PaintingService(CommandHistory& commandHistory);
        ~PaintingService() = default;

        // Tool selection
        void SetTool(PaintingTool tool) { m_CurrentTool = tool; }
        PaintingTool GetTool() const { return m_CurrentTool; }

        // Brush management
        void SetBrush(const Domain::TileData& brush) { m_CurrentBrush = brush; }
        const Domain::TileData& GetBrush() const { return m_CurrentBrush; }
        Domain::TileData& GetBrush() { return m_CurrentBrush; }

        // Working layer
        void SetWorkingLayerIndex(size_t index) { m_WorkingLayerIndex = index; }
        size_t GetWorkingLayerIndex() const { return m_WorkingLayerIndex; }

        // Painting operations (these create commands)
        void Paint(Domain::TileProject& project, const Domain::Position& position);
        void Erase(Domain::TileProject& project, const Domain::Position& position);
        void Fill(Domain::TileProject& project, const Domain::Position& position);

        // Direct painting (no undo - used for previews)
        void PaintDirect(Domain::TileGrid& layer, const Domain::Position& position, const Domain::TileData& tile);

        // Begin/End stroke (for merging continuous painting into single undo)
        void BeginStroke();
        void EndStroke();
        bool IsInStroke() const { return m_InStroke; }

    private:
        void FloodFill(Domain::TileGrid& layer, const Domain::Position& startPos, const Domain::TileData& newTile);

        CommandHistory& m_CommandHistory;
        PaintingTool m_CurrentTool = PaintingTool::None;
        Domain::TileData m_CurrentBrush;
        size_t m_WorkingLayerIndex = 0;
        bool m_InStroke = false;
    };
}
