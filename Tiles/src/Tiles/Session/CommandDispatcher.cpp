#include "CommandDispatcher.h"

namespace Tiles
{
    void CommandDispatcher::Execute(std::unique_ptr<Command> command, LayerStack& layerStack)
    {
        if (!command)
            return;

        m_History.Execute(std::move(command), layerStack);

        if (m_OnMutated)
            m_OnMutated();
    }

    void CommandDispatcher::Undo(LayerStack& layerStack)
    {
        if (!m_History.CanUndo())
            return;

        m_History.Undo(layerStack);

        if (m_OnMutated)
            m_OnMutated();
    }

    void CommandDispatcher::Redo(LayerStack& layerStack)
    {
        if (!m_History.CanRedo())
            return;

        m_History.Redo(layerStack);

        if (m_OnMutated)
            m_OnMutated();
    }
}
