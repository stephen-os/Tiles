#pragma once
#include "Command.h"
#include "../Tile.h"
#include "../LayerStack.h"

namespace Tiles
{
    class TilePaintCommand : public Command
    {
    public:
        TilePaintCommand(size_t x, size_t y, size_t index, const Tile& newTile)
            : m_X(x), m_Y(y), m_Index(index), m_NewTile(newTile), m_HasExecuted(false)
        {
        }

        virtual void Execute(LayerStack& layerStack) override
        {
            Tile& currentTile = layerStack.GetTile(m_X, m_Y, m_Index);

            if (!m_HasExecuted)
            {
                m_PreviousTile = currentTile;
                m_HasExecuted = true;
            }

            if (currentTile == m_NewTile)
                return;

            currentTile = m_NewTile;
        }

        virtual void Undo(LayerStack& layerStack) override
        {
            Tile& currentTile = layerStack.GetTile(m_X, m_Y, m_Index);
            currentTile = m_PreviousTile;
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
        size_t m_X, m_Y, m_Index;
        Tile m_PreviousTile;
        Tile m_NewTile;
        bool m_HasExecuted;
    };
}