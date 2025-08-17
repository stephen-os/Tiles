#pragma once

#include "Command.h"

#include "../LayerStack.h"

namespace Tiles
{
    class LayerSwapCommand : public Command
    {
    public:
        LayerSwapCommand(size_t indexA, size_t indexB)
            : m_IndexA(indexA), m_IndexB(indexB)
        {
        }

        virtual void Execute(LayerStack& layerStack) override
        {
            layerStack.SwapLayers(m_IndexA, m_IndexB);
        }

        virtual void Undo(LayerStack& layerStack) override
        {
            layerStack.SwapLayers(m_IndexA, m_IndexB);
        }

        virtual bool Validate(const Command& other) const override
        {
            return false;
        }

    private:
        size_t m_IndexA;
        size_t m_IndexB;
    };
}