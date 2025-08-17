#pragma once

#include <queue>

#include "Command.h"

#include "../Tile.h"
#include "../TileLayer.h"

namespace Tiles
{
    class LayerFillCommand : public Command
    {
    public:
        LayerFillCommand(size_t x, size_t y, size_t index, const Tile& fillTile)
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

            const Tile& targetTile = layer.GetTile(m_X, m_Y);
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
        void FloodFill(TileLayer& layer, size_t startX, size_t startY, const Tile& targetTile)
        {
            std::queue<std::pair<size_t, size_t>> tileQueue;
            tileQueue.push({ startX, startY });

            const std::vector<std::pair<int, int>> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

            while (!tileQueue.empty())
            {
                auto [x, y] = tileQueue.front();
                tileQueue.pop();

                if (!layer.IsValidPosition(x, y))
                    continue;

                Tile& tile = layer.GetTile(x, y);
                if (tile != targetTile)
                    continue;

                tile = m_FillTile;

                for (const auto& [dx, dy] : directions)
                {
                    int newX = static_cast<int>(x) + dx;
                    int newY = static_cast<int>(y) + dy;

                    if (newX >= 0 && newY >= 0)
                    {
                        tileQueue.push({ static_cast<size_t>(newX), static_cast<size_t>(newY) });
                    }
                }
            }
        }

        size_t m_X, m_Y, m_Index;
        Tile m_FillTile;
        TileLayer m_PreviousLayer;
        bool m_HasExecuted;
    };
}