#pragma once

#include <queue>
#include <algorithm>

#include "Commands/Command.h"

#include "Domain/Tile.h"
#include "Domain/TileLayer.h"

namespace Tiles
{
    /// Flood-fills the contiguous region matching the tile at (x, y) with a fill
    /// tile, snapshotting the whole layer on first Execute so Undo can restore it.
    class LayerFillCommand : public Command
    {
    public:
        LayerFillCommand(int x, int y, size_t index, const Tile& fillTile)
            : m_X(x), m_Y(y), m_Index(index), m_FillTile(fillTile), m_HasExecuted(false)
        {
        }

        virtual void Execute(LayerStack& layerStack) override
        {
            TileLayer& layer = layerStack.GetLayer(m_Index);

            if (!m_HasExecuted)
            {
                m_PreviousLayer = layer;
                m_HasExecuted = true;
            }

            const Tile targetTile = layer.GetTile(m_X, m_Y);
            if (targetTile == m_FillTile)
                return;

            TileLayer newLayer = m_PreviousLayer;
            FloodFill(newLayer, m_X, m_Y, targetTile);

            layerStack.ReplaceLayer(m_Index, newLayer);
        }

        virtual void Undo(LayerStack& layerStack) override
        {
            layerStack.ReplaceLayer(m_Index, m_PreviousLayer);
        }

        virtual bool Validate(const Command& other) const override
        {
            return false;
        }

    private:
        /// Breadth-first 4-connected fill: replaces every tile reachable from
        /// (startX, startY) that equals targetTile with the fill tile. The board is
        /// unbounded, so the flood is confined to the layer's painted bounding box
        /// expanded to include the start cell; this keeps filling an empty region
        /// finite while still filling enclosed gaps within painted content.
        void FloodFill(TileLayer& layer, int startX, int startY, const Tile& targetTile)
        {
            // Bound the flood to the layer's painted extent. A click on empty
            // space outside that extent (or on an empty layer) fills only the
            // clicked cell, so a fill can never run away across the unbounded
            // board -- the safety net for an infinite canvas.
            auto bounds = layer.GetBounds();
            if (!bounds
                || startX < bounds->x || startX > bounds->z
                || startY < bounds->y || startY > bounds->w)
            {
                layer.SetTile(startX, startY, m_FillTile);
                return;
            }

            const int minX = bounds->x, minY = bounds->y, maxX = bounds->z, maxY = bounds->w;

            std::queue<std::pair<int, int>> tileQueue;
            tileQueue.push({ startX, startY });

            const std::pair<int, int> directions[] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

            // Backstop against a pathologically large (sparse) bounding box.
            size_t filled = 0;
            constexpr size_t MaxFillCells = 1'000'000;

            while (!tileQueue.empty())
            {
                auto [x, y] = tileQueue.front();
                tileQueue.pop();

                if (x < minX || x > maxX || y < minY || y > maxY)
                    continue;

                // Already filled cells no longer equal targetTile, ending each branch.
                if (layer.GetTile(x, y) != targetTile)
                    continue;

                layer.SetTile(x, y, m_FillTile);
                if (++filled >= MaxFillCells)
                    break;

                for (const auto& [dx, dy] : directions)
                    tileQueue.push({ x + dx, y + dy });
            }
        }

        int m_X, m_Y;
        size_t m_Index;
        Tile m_FillTile;
        TileLayer m_PreviousLayer;
        bool m_HasExecuted;
    };
}