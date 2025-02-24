#pragma once

#include "Command.h"

#include "../Core/Layers.h"
#include "../Core/Layer.h"
#include <iostream>

namespace Tiles
{
	class RemoveLayerCommand : public Command
	{
	public:
		RemoveLayerCommand(const size_t& index, Layer& previousLayer)
		{
			m_Index = index;
			m_PreviousLayer = previousLayer;
		}

		virtual void Execute(Layers& layers) override
		{
			layers.RemoveLayer(m_Index);
		}

		virtual void Undo(Layers& layers) override
		{
			layers.InsertLayer(m_Index, m_PreviousLayer);
		}
	private:
		size_t m_Index;
		Layer m_PreviousLayer; 
	};
}