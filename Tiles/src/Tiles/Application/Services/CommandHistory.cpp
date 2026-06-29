#include "CommandHistory.h"

namespace Tiles::Services
{
    CommandHistory::CommandHistory(size_t maxHistorySize)
        : m_MaxHistorySize(maxHistorySize)
    {
    }

    void CommandHistory::Execute(CommandPtr command)
    {
        if (!command)
            return;

        // Execute the command
        command->Execute();

        // Check if we can merge with the previous command
        if (!m_UndoStack.empty() && command->CanMergeWith(*m_UndoStack.back()))
        {
            m_UndoStack.back()->MergeWith(*command);
        }
        else
        {
            m_UndoStack.push_back(std::move(command));
            TrimHistory();
        }

        // Clear redo stack when new command is executed
        m_RedoStack.clear();

        NotifyHistoryChanged();
    }

    void CommandHistory::Undo()
    {
        if (!CanUndo())
            return;

        auto command = std::move(m_UndoStack.back());
        m_UndoStack.pop_back();

        command->Undo();
        m_RedoStack.push_back(std::move(command));

        NotifyHistoryChanged();
    }

    void CommandHistory::Redo()
    {
        if (!CanRedo())
            return;

        auto command = std::move(m_RedoStack.back());
        m_RedoStack.pop_back();

        command->Execute();
        m_UndoStack.push_back(std::move(command));

        NotifyHistoryChanged();
    }

    std::string CommandHistory::GetUndoDescription() const
    {
        if (m_UndoStack.empty())
            return "";
        return m_UndoStack.back()->GetDescription();
    }

    std::string CommandHistory::GetRedoDescription() const
    {
        if (m_RedoStack.empty())
            return "";
        return m_RedoStack.back()->GetDescription();
    }

    void CommandHistory::Clear()
    {
        m_UndoStack.clear();
        m_RedoStack.clear();
        NotifyHistoryChanged();
    }

    void CommandHistory::NotifyHistoryChanged()
    {
        if (m_OnHistoryChanged)
        {
            m_OnHistoryChanged();
        }
    }

    void CommandHistory::TrimHistory()
    {
        while (m_UndoStack.size() > m_MaxHistorySize)
        {
            m_UndoStack.erase(m_UndoStack.begin());
        }
    }
}
