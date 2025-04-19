#pragma once

#include "Command.h"

#include "../Core/Layers.h"
#include "../Core/Layer.h"

#include "Lumina/Core/Log.h"

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
			LUMINA_LOG_INFO("Adding layer at index: {0}", m_Index);
			layers.InsertLayer(m_Index, layer);
		}

		virtual void Undo(Layers& layers) override
		{
			LUMINA_LOG_INFO("Removing layer at index: {0}", m_Index);
			layers.RemoveLayer(m_Index);
		}

		virtual bool Validate(const Command& other) const override
		{
			const AddLayerCommand* otherCmd = dynamic_cast<const AddLayerCommand*>(&other);
			if (!otherCmd)
				return false;

			return m_Index == otherCmd->m_Index;
		}

	private:
		size_t m_Index;
	};
}