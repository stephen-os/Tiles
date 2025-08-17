#pragma once
#include "Command.h"
#include "../LayerStack.h"

namespace Tiles
{
    class LayerAddCommand : public Command
    {
    public:
        LayerAddCommand(const std::string& layerName = "")
            : m_Index(0), m_LayerName(layerName), m_HasExecuted(false)
        {
        }

        virtual void Execute(LayerStack& layerStack) override
        {
            if (!m_HasExecuted)
            {
                m_Index = layerStack.GetLayerCount();

                if (m_LayerName.empty())
                {
                    m_LayerName = "Layer " + std::to_string(m_Index);
                }

                m_HasExecuted = true;
            }

            layerStack.AddLayer(m_LayerName);
        }

        virtual void Undo(LayerStack& layerStack) override
        {
            layerStack.RemoveLayer(m_Index);
        }

        virtual bool Validate(const Command& other) const override
        {
            return false;
        }

    private:
        size_t m_Index;
        std::string m_LayerName;
        bool m_HasExecuted;
    };
}