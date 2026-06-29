#pragma once

#include "../Commands/ICommand.h"

#include <vector>
#include <functional>

namespace Tiles::Services
{
    // Service for managing command history (undo/redo)
    class CommandHistory
    {
    public:
        using HistoryChangedCallback = std::function<void()>;

        CommandHistory(size_t maxHistorySize = 100);
        ~CommandHistory() = default;

        // Execute a command and add it to history
        void Execute(CommandPtr command);

        // Undo the last command
        void Undo();

        // Redo the last undone command
        void Redo();

        // Query state
        bool CanUndo() const { return !m_UndoStack.empty(); }
        bool CanRedo() const { return !m_RedoStack.empty(); }

        // Get descriptions for UI
        std::string GetUndoDescription() const;
        std::string GetRedoDescription() const;

        // Clear all history
        void Clear();

        // Get history size
        size_t GetUndoCount() const { return m_UndoStack.size(); }
        size_t GetRedoCount() const { return m_RedoStack.size(); }

        // Callback when history changes
        void SetHistoryChangedCallback(HistoryChangedCallback callback) { m_OnHistoryChanged = std::move(callback); }

    private:
        void NotifyHistoryChanged();
        void TrimHistory();

        std::vector<CommandPtr> m_UndoStack;
        std::vector<CommandPtr> m_RedoStack;
        size_t m_MaxHistorySize;
        HistoryChangedCallback m_OnHistoryChanged;
    };
}
