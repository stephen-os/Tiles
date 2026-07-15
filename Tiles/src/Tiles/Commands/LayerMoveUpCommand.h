#pragma once

#include "Commands/Command.h"

#include "Domain/LayerStack.h"

namespace Tiles
{
	// Moves a layer one position up in the stack; Undo moves it back down.
	class LayerMoveUpCommand : public Command
	{
	public:
		LayerMoveUpCommand(size_t layerIndex)
			: m_Index(layerIndex)
		{
		}

		// Moves the layer up one position.
		void Execute(LayerStack& layerStack) override
		{
			layerStack.MoveLayerUp(m_Index);
		}

		// Moves the layer back down to its original position.
		void Undo(LayerStack& layerStack) override
		{
			if (m_Index > 0)
				layerStack.MoveLayerDown(m_Index - 1);
		}

	private:
		size_t m_Index;
	};
}
