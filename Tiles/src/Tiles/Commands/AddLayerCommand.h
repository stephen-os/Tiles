#pragma once

#pragma once

#include "Command.h"

#include "../Core/Layers.h"
#include "../Core/Layer.h"

#include <iostream>
#include <string>

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
			Layer layer(layers.GetWidth(), layers.GetHeight(), std::string("Layer " + std::to_string(m_Index)));
			layers.InsertLayer(m_Index, layer);
		}

		virtual void Undo(Layers& layers) override
		{
			layers.RemoveLayer(m_Index);
		}
	private:
		size_t m_Index;
	};
}