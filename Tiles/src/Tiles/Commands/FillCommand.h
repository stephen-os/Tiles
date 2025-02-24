#pragma once

#include "Command.h"

#include "../Core/Layers.h"
#include "../Core/Layer.h"
#include <iostream>

namespace Tiles
{
	class FillCommand : public Command
	{
	public:
		FillCommand(const size_t& index, const Layer& previousLayer, const Layer& newLayer)
		{
			m_Index = index;
			m_PreviousLayer = previousLayer;
			m_NewLayer = newLayer;
		}

		virtual void Execute(Layers& layers) override
		{
			layers.ReplaceLayer(m_Index, m_NewLayer);
		}

		virtual void Undo(Layers& layers) override
		{
			layers.ReplaceLayer(m_Index, m_PreviousLayer);
		}
	private:
		size_t m_Index;
		Layer m_PreviousLayer;
		Layer m_NewLayer;
	};
}