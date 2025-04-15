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
			LUMINA_LOG_INFO("AddLayerCommand: {0}", index);

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

		virtual bool Validate(const Command& other) const override
		{
			return false;

			if (auto otherCommand = dynamic_cast<const AddLayerCommand*>(&other))
			{
				return m_Index == otherCommand->m_Index;
			}
			return false;
		}

	private:
		size_t m_Index;
	};
}