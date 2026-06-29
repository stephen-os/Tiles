#include "PaintingService.h"
#include "../Commands/PaintCommand.h"

#include <array>
#include <queue>
#include <unordered_set>

namespace Tiles::Services
{
    PaintingService::PaintingService(CommandHistory& commandHistory)
        : m_CommandHistory(commandHistory)
    {
    }

    void PaintingService::Paint(Domain::TileProject& project, const Domain::Position& position)
    {
        if (m_WorkingLayerIndex >= project.GetLayerCount())
            return;

        auto& layer = project.GetLayer(m_WorkingLayerIndex);
        if (!layer.IsInBounds(position))
            return;

        auto command = std::make_unique<PaintTileCommand>(layer, position, m_CurrentBrush);
        m_CommandHistory.Execute(std::move(command));
        project.MarkAsModified();
    }

    void PaintingService::Erase(Domain::TileProject& project, const Domain::Position& position)
    {
        if (m_WorkingLayerIndex >= project.GetLayerCount())
            return;

        auto& layer = project.GetLayer(m_WorkingLayerIndex);
        if (!layer.IsInBounds(position))
            return;

        auto command = std::make_unique<EraseTileCommand>(layer, position);
        m_CommandHistory.Execute(std::move(command));
        project.MarkAsModified();
    }

    void PaintingService::Fill(Domain::TileProject& project, const Domain::Position& position)
    {
        if (m_WorkingLayerIndex >= project.GetLayerCount())
            return;

        auto& layer = project.GetLayer(m_WorkingLayerIndex);
        if (!layer.IsInBounds(position))
            return;

        // Create a stroke command to capture all fill changes
        auto command = std::make_unique<PaintStrokeCommand>(layer);

        // Get the target tile to replace
        const auto& targetTile = layer.GetTile(position);

        // Don't fill if the target is the same as the brush
        if (targetTile == m_CurrentBrush)
            return;

        // Flood fill using BFS
        std::queue<Domain::Position> queue;
        std::unordered_set<Domain::Position> visited;

        queue.push(position);
        visited.insert(position);

        while (!queue.empty())
        {
            Domain::Position current = queue.front();
            queue.pop();

            const auto& currentTile = layer.GetTile(current);
            if (currentTile != targetTile)
                continue;

            command->AddTile(current, m_CurrentBrush);

            // Check neighbors (4-directional)
            const std::array<Domain::Position, 4> neighbors = {{
                Domain::Position(current.X > 0 ? current.X - 1 : 0u, current.Y),
                Domain::Position(current.X + 1, current.Y),
                Domain::Position(current.X, current.Y > 0 ? current.Y - 1 : 0u),
                Domain::Position(current.X, current.Y + 1)
            }};

            for (const auto& neighbor : neighbors)
            {
                if (layer.IsInBounds(neighbor) && visited.find(neighbor) == visited.end())
                {
                    visited.insert(neighbor);
                    if (layer.GetTile(neighbor) == targetTile)
                    {
                        queue.push(neighbor);
                    }
                }
            }
        }

        if (!command->IsEmpty())
        {
            m_CommandHistory.Execute(std::move(command));
            project.MarkAsModified();
        }
    }

    void PaintingService::PaintDirect(Domain::TileGrid& layer, const Domain::Position& position,
                                       const Domain::TileData& tile)
    {
        layer.SetTile(position, tile);
    }

    void PaintingService::BeginStroke()
    {
        m_InStroke = true;
    }

    void PaintingService::EndStroke()
    {
        m_InStroke = false;
    }

    void PaintingService::FloodFill(Domain::TileGrid& layer, const Domain::Position& startPos,
                                     const Domain::TileData& newTile)
    {
        // This is now handled in the Fill method with commands
    }
}
