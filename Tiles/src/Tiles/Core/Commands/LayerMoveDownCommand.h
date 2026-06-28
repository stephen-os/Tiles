#pragma once

#include "Command.h"

#include "../LayerStack.h"

namespace Tiles
{
    class LayerMoveDownCommand : public Command
    {
    public:
        LayerMoveDownCommand(size_t layerIndex)
            : m_Index(layerIndex)
        {
        }

        virtual void Execute(LayerStack& layerStack) override
        {
            layerStack.MoveLayerDown(m_Index);
        }

        virtual void Undo(LayerStack& layerStack) override
        {
            if (m_Index < layerStack.GetLayerCount() - 1)
            {
                layerStack.MoveLayerUp(m_Index + 1);
            }
        }

        virtual bool Validate(const Command& other) const override
        {
            return false;
        }

    private:
        size_t m_Index;
    };
}
