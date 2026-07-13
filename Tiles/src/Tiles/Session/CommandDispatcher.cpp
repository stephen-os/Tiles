#include "CommandDispatcher.h"

namespace Tiles
{
	// Runs a non-null command through the history, then fires the mutation hook.
	void CommandDispatcher::Execute(std::unique_ptr<Command> command, LayerStack& layerStack)
	{
		if (!command)
			return;

		m_History.Execute(std::move(command), layerStack);

		if (m_OnMutated)
			m_OnMutated();
	}

	// Reverts the most recent command and fires the mutation hook.
	void CommandDispatcher::Undo(LayerStack& layerStack)
	{
		if (!m_History.CanUndo())
			return;

		m_History.Undo(layerStack);

		if (m_OnMutated)
			m_OnMutated();
	}

	// Re-applies the most recently undone command and fires the mutation hook.
	void CommandDispatcher::Redo(LayerStack& layerStack)
	{
		if (!m_History.CanRedo())
			return;

		m_History.Redo(layerStack);

		if (m_OnMutated)
			m_OnMutated();
	}
}
