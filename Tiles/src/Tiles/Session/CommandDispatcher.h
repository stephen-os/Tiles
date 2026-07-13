#pragma once

#include <functional>
#include <memory>

#include "Commands/CommandHistory.h"

namespace Tiles
{
	class Command;
	class LayerStack;

	// Owns the undo/redo history and runs commands against a layer stack. A
	// caller-supplied hook fires after any successful execute/undo/redo, so the
	// owner performs its post-mutation bookkeeping (mark the project modified,
	// revalidate the working layer) in one place instead of at every call site.
	class CommandDispatcher
	{
	public:
		using MutationHook = std::function<void()>;

		// Sets the hook run after any successful execute/undo/redo.
		void SetOnMutated(MutationHook hook) { m_OnMutated = std::move(hook); }

		// Runs a non-null command through the history; a null command is ignored.
		void Execute(std::unique_ptr<Command> command, LayerStack& layerStack);

		// Reverts the most recent command, if any.
		void Undo(LayerStack& layerStack);

		// Re-applies the most recently undone command, if any.
		void Redo(LayerStack& layerStack);

		// True when there is a command to undo.
		[[nodiscard]] bool CanUndo() const { return m_History.CanUndo(); }

		// True when there is a command to redo.
		[[nodiscard]] bool CanRedo() const { return m_History.CanRedo(); }

		// Drops the entire undo/redo history.
		void Clear() { m_History.Clear(); }

	private:
		CommandHistory m_History;
		MutationHook m_OnMutated;
	};
}
