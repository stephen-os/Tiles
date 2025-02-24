#pragma once

#pragma once

#include "Command.h"

#include "../Core/Layers.h"
#include "../Core/Layer.h"
#include <iostream>

namespace Tiles
{
	class AddLayerCommand : public Command
	{
	public:
		AddLayerCommand(const size_t& index)
		{
			m_Index = index;
		}

		virtual void Execute(Layers& layers) override
		{
			layers.AddLayer();
		}

		virtual void Undo(Layers& layers) override
		{
			layers.RemoveLayer(m_Index);
		}
	private:
		size_t m_Index;
	};
}