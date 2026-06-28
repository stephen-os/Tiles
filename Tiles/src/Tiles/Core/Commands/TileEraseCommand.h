#pragma once
#include "Command.h"
#include "../Tile.h"
#include "../LayerStack.h"

namespace Tiles
{
    class TileEraseCommand : public Command
    {
    public:
        TileEraseCommand(size_t x, size_t y, size_t index)
            : m_X(x), m_Y(y), m_Index(index), m_HasExecuted(false)
        {
        }

        virtual void Execute(LayerStack& layerStack) override
        {
            Tile& currentTile = layerStack.GetTile(m_X, m_Y, m_Index);

            if (!m_HasExecuted)
            {
                m_PreviousTile = currentTile;  // Capture on first execution
                m_HasExecuted = true;
            }

            Tile emptyTile;
            if (currentTile == emptyTile)
                return;

            currentTile.Reset();
        }

        virtual void Undo(LayerStack& layerStack) override
        {
            Tile& currentTile = layerStack.GetTile(m_X, m_Y, m_Index);
            currentTile = m_PreviousTile;
        }

        virtual bool Validate(const Command& other) const override
        {
            const TileEraseCommand* otherCmd = dynamic_cast<const TileEraseCommand*>(&other);
            if (!otherCmd)
                return false;
            return m_X == otherCmd->m_X && m_Y == otherCmd->m_Y && m_Index == otherCmd->m_Index;
        }

    private:
        size_t m_X, m_Y, m_Index;
        Tile m_PreviousTile;
        bool m_HasExecuted;
    };
}