#pragma once
#include "Commands/Command.h"
#include "Domain/Tile.h"
#include "Domain/LayerStack.h"

namespace Tiles
{
    /// Paints a single tile, capturing the overwritten tile on first Execute so
    /// Undo can restore it. Coalesces with an identical paint on the same cell.
    class TilePaintCommand : public Command
    {
    public:
        TilePaintCommand(int x, int y, size_t index, const Tile& newTile)
            : m_X(x), m_Y(y), m_Index(index), m_NewTile(newTile), m_HasExecuted(false)
        {
        }

        virtual void Execute(LayerStack& layerStack) override
        {
            const Tile& currentTile = layerStack.GetTile(m_X, m_Y, m_Index);

            if (!m_HasExecuted)
            {
                m_PreviousTile = currentTile;
                m_HasExecuted = true;
            }

            if (currentTile == m_NewTile)
                return;

            layerStack.SetTile(m_X, m_Y, m_Index, m_NewTile);
        }

        virtual void Undo(LayerStack& layerStack) override
        {
            layerStack.SetTile(m_X, m_Y, m_Index, m_PreviousTile);
        }

        virtual bool Validate(const Command& other) const override
        {
            const TilePaintCommand* otherCmd = dynamic_cast<const TilePaintCommand*>(&other);
            if (!otherCmd)
                return false;
            return m_X == otherCmd->m_X && m_Y == otherCmd->m_Y &&
                m_Index == otherCmd->m_Index && m_NewTile == otherCmd->m_NewTile;
        }

    private:
        int m_X, m_Y;
        size_t m_Index;
        Tile m_PreviousTile;
        Tile m_NewTile;
        bool m_HasExecuted;
    };
}