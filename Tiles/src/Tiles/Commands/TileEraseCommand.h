#pragma once
#include "Commands/Command.h"
#include "Domain/Tile.h"
#include "Domain/LayerStack.h"

namespace Tiles
{
    // Resets a single tile to empty, capturing its prior state on first Execute
    // for Undo. Coalesces with an identical erase on the same cell.
    class TileEraseCommand : public Command
    {
    public:
        TileEraseCommand(int x, int y, size_t index)
            : m_X(x), m_Y(y), m_Index(index), m_HasExecuted(false)
        {
        }

        virtual void Execute(LayerStack& layerStack) override
        {
            const Tile& currentTile = layerStack.GetTile(m_X, m_Y, m_Index);

            if (!m_HasExecuted)
            {
                m_PreviousTile = currentTile;  // Capture on first execution
                m_HasExecuted = true;
            }

            Tile emptyTile;
            if (currentTile == emptyTile)
                return;

            // An unpainted tile clears the sparse cell.
            layerStack.SetTile(m_X, m_Y, m_Index, Tile());
        }

        virtual void Undo(LayerStack& layerStack) override
        {
            layerStack.SetTile(m_X, m_Y, m_Index, m_PreviousTile);
        }

        virtual bool Validate(const Command& other) const override
        {
            const TileEraseCommand* otherCmd = dynamic_cast<const TileEraseCommand*>(&other);
            if (!otherCmd)
                return false;
            return m_X == otherCmd->m_X && m_Y == otherCmd->m_Y && m_Index == otherCmd->m_Index;
        }

    private:
        int m_X, m_Y;
        size_t m_Index;
        Tile m_PreviousTile;
        bool m_HasExecuted;
    };
}