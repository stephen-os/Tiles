#pragma once

#include "Command.h"

#include "../LayerStack.h"

namespace Tiles
{
    class LayerInsertCommand : public Command
    {
    public:
        LayerInsertCommand(size_t insertIndex, const std::string& layerName = "")
            : m_InsertIndex(insertIndex), m_LayerName(layerName), m_HasExecuted(false)
        {
        }

        virtual void Execute(LayerStack& layerStack) override
        {
            if (!m_HasExecuted)
            {
                if (m_LayerName.empty())
                {
                    m_LayerName = "Layer " + std::to_string(m_InsertIndex);
                }
                m_HasExecuted = true;
            }

            layerStack.InsertLayer(m_InsertIndex, m_LayerName);
        }

        virtual void Undo(LayerStack& layerStack) override
        {
            layerStack.RemoveLayer(m_InsertIndex);
        }

        virtual bool Validate(const Command& other) const override
        {
            return false;
        }

    private:
        size_t m_InsertIndex;
        std::string m_LayerName;
        bool m_HasExecuted;
    };
}