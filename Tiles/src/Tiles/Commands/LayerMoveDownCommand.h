#pragma once

#include "Commands/Command.h"

#include "Domain/LayerStack.h"

namespace Tiles
{
	// Moves a layer one position down in the stack; Undo moves it back up.
	class LayerMoveDownCommand : public Command
	{
	public:
		LayerMoveDownCommand(size_t layerIndex)
			: m_Index(layerIndex)
		{
		}

		// Moves the layer down one position.
		void Execute(LayerStack& layerStack) override
		{
			layerStack.MoveLayerDown(m_Index);
		}

		// Moves the layer back up to its original position.
		void Undo(LayerStack& layerStack) override
		{
			if (m_Index < layerStack.GetLayerCount() - 1)
				layerStack.MoveLayerUp(m_Index + 1);
		}

	private:
		size_t m_Index;
	};
}
