#pragma once

#include <deque>
#include <memory>

#include "LayerStack.h"

#include "Commands/Command.h"

namespace Tiles
{
    class Command;
    class LayerStack;

    static const size_t MAX_UNDO_STACK_SIZE = 1000;

    class CommandHistory
    {
    public:
        CommandHistory() = default;

        void Execute(std::unique_ptr<Command> command, LayerStack& layerStack);
        
        void Undo(LayerStack& layerStack);
        void Redo(LayerStack& layerStack);
        
        bool CanUndo() const { return !m_UndoStack.empty(); }
        bool CanRedo() const { return !m_RedoStack.empty(); }
        
        void Clear();

    private:
        std::deque<std::unique_ptr<Command>> m_UndoStack;
        std::deque<std::unique_ptr<Command>> m_RedoStack;
    };
}