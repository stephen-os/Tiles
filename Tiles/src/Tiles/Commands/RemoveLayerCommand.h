#pragma once

#include "Command.h"

#include "../Core/Layers.h"
#include "../Core/Layer.h"

#include "Lumina/Core/Log.h"

namespace Tiles
{
	class RemoveLayerCommand : public Command
	{
	public:
		RemoveLayerCommand(const size_t& index, Layer& previousLayer)
		{
			LUMINA_LOG_INFO("RemoveLayerCommand: {0}", index);

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

		virtual bool Validate(const Command& other) const override
		{
			return false; 

			if (auto otherCommand = dynamic_cast<const RemoveLayerCommand*>(&other))
			{
				return m_Index == otherCommand->m_Index;
			}
			return false;
		}

	private:
		size_t m_Index;
		Layer m_PreviousLayer; 
	};
}