#pragma once

#include "Lumina/Core/Aliases.h"
#include "Lumina/Core/Log.h"

#include <deque>
#include "Command.h"

#include "../Core/Layers.h"


namespace Tiles
{
	class CommandHistory
	{
	public:
		CommandHistory(Shared<Layers> layers) : m_Layers(layers) {}

		void ExecuteCommand(Unique<Command> command)
		{
			if (!m_UndoStack.empty() && m_UndoStack.back()->Validate(*command))
				return;

			command->Execute(*m_Layers);
			m_UndoStack.push_back(std::move(command));
			m_RedoStack.clear();
		}

		void Undo()
		{
			if (m_UndoStack.empty()) return;

			Unique<Command> command = std::move(m_UndoStack.back());
			m_UndoStack.pop_back();

			command->Undo(*m_Layers);
			m_RedoStack.push_back(std::move(command));
		}

		void Redo()
		{
			if (m_RedoStack.empty()) return;

			Unique<Command> command = std::move(m_RedoStack.back());
			m_RedoStack.pop_back();

			command->Execute(*m_Layers);
			m_UndoStack.push_back(std::move(command));
		}

		bool CanUndo() const { return !m_UndoStack.empty(); }
		bool CanRedo() const { return !m_RedoStack.empty(); }

	private:
		Shared<Layers> m_Layers;
		std::deque<Unique<Command>> m_UndoStack;
		std::deque<Unique<Command>> m_RedoStack;
	};
}
