#pragma once

#include "Commands/Command.h"

#include "Domain/LayerStack.h"

#include <deque>
#include <memory>

namespace Tiles
{
	inline constexpr size_t MAX_UNDO_STACK_SIZE = 1000;

	// Undo/redo stack over LayerStack edits. Executing a fresh command clears
	// the redo stack; the undo stack is capped at MAX_UNDO_STACK_SIZE.
	class CommandHistory
	{
	public:
		CommandHistory() = default;

		// Runs command against layerStack and pushes it onto the undo stack. If
		// the previous command reports it as a redundant repeat (Command::
		// CanCoalesce), the command is dropped to coalesce identical edits.
		void Execute(std::unique_ptr<Command> command, LayerStack& layerStack);

		void Undo(LayerStack& layerStack);
		void Redo(LayerStack& layerStack);

		[[nodiscard]] bool CanUndo() const { return !m_UndoStack.empty(); }
		[[nodiscard]] bool CanRedo() const { return !m_RedoStack.empty(); }

		void Clear();

	private:
		std::deque<std::unique_ptr<Command>> m_UndoStack;
		std::deque<std::unique_ptr<Command>> m_RedoStack;
	};
}
