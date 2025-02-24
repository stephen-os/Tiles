#pragma once

#include <deque>
#include <memory>
#include "Command.h"
#include "../Core/Layers.h"
#include "../Core/Base.h"

#include <iostream>

namespace Tiles
{
	class CommandHistory
	{
	public:
		CommandHistory(Shared<Layers> layers) : m_Layers(layers) {}

		void ExecuteCommand(Unique<Command> command)
		{
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
