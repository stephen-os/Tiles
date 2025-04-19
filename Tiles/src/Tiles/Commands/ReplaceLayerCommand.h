#pragma once

#include "Command.h"

#include "../Core/Layers.h"
#include "../Core/Layer.h"

#include "Lumina/Core/Log.h"

namespace Tiles
{
	class ReplaceLayerCommand : public Command
	{
	public:
		ReplaceLayerCommand(const size_t& index, const Layer& previousLayer, const Layer& newLayer)
		{
			m_Index = index;
			m_PreviousLayer = previousLayer;
			m_NewLayer = newLayer;
		}

		virtual void Execute(Layers& layers) override
		{
			LUMINA_LOG_INFO("Replacing layer at index: {0}", m_Index);
			layers.ReplaceLayer(m_Index, m_NewLayer);
		}

		virtual void Undo(Layers& layers) override
		{
			LUMINA_LOG_INFO("Undoing replace layer at index: {0}", m_Index);
			layers.ReplaceLayer(m_Index, m_PreviousLayer);
		}

		virtual bool Validate(const Command& other) const override
		{
			if (auto otherCommand = dynamic_cast<const ReplaceLayerCommand*>(&other))
			{
				return m_Index == otherCommand->m_Index;
			}
			return false;
		}

	private:
		size_t m_Index;
		Layer m_PreviousLayer;
		Layer m_NewLayer;
	};
}