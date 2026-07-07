#pragma once
#include "Commands/Command.h"
#include "Domain/LayerStack.h"
#include "Domain/TileLayer.h"

namespace Tiles
{
    // Clears every tile on a layer, snapshotting the layer on first Execute so
    // Undo can restore its contents.
    class LayerClearCommand : public Command
    {
    public:
        LayerClearCommand(size_t index)
            : m_Index(index), m_HasExecuted(false)
        {
        }

        virtual void Execute(LayerStack& layerStack) override
        {
            if (!m_HasExecuted)
            {
                m_PreviousLayer = layerStack.GetLayer(m_Index);
                m_HasExecuted = true;
            }

            layerStack.ClearLayer(m_Index);
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
        size_t m_Index;
        TileLayer m_PreviousLayer;
        bool m_HasExecuted;
    };
}