#include "CommandHistory.h"

#include "Commands/Command.h"

namespace Tiles
{
    void CommandHistory::Execute(std::unique_ptr<Command> command, LayerStack& layerStack)
    {
        if (!command)
            return;

        if (!m_UndoStack.empty() && m_UndoStack.back()->Validate(*command))
            return;

        command->Execute(layerStack);
        m_UndoStack.push_back(std::move(command));

        if (m_UndoStack.size() > MAX_UNDO_STACK_SIZE)
            m_UndoStack.pop_front();

        m_RedoStack.clear();
    }

    void CommandHistory::Undo(LayerStack& layerStack)
    {
        if (m_UndoStack.empty())
            return;

        std::unique_ptr<Command> command = std::move(m_UndoStack.back());
        m_UndoStack.pop_back();

        command->Undo(layerStack);

        m_RedoStack.push_back(std::move(command));

        if (m_RedoStack.size() > MAX_UNDO_STACK_SIZE)
            m_RedoStack.pop_front();
    }

    void CommandHistory::Redo(LayerStack& layerStack)
    {
        if (m_RedoStack.empty())
            return;

        std::unique_ptr<Command> command = std::move(m_RedoStack.back());
        m_RedoStack.pop_back();

        command->Execute(layerStack);

        m_UndoStack.push_back(std::move(command));

        if (m_UndoStack.size() > MAX_UNDO_STACK_SIZE)
            m_UndoStack.pop_front();
    }

    void CommandHistory::Clear()
    {
        m_UndoStack.clear();
        m_RedoStack.clear();
    }
}