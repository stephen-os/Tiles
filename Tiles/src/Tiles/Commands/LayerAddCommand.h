#pragma once
#include "Commands/Command.h"
#include "Domain/LayerStack.h"

namespace Tiles
{
    /// Appends a new layer to the top of the stack. On first Execute it records
    /// the assigned index (and a default name if none was given) so Undo can
    /// remove exactly that layer.
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