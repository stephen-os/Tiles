#pragma once

#include "Command.h"

#include "../Core/Layers.h"
#include "../Core/Layer.h"

#include "Lumina/Core/Log.h"

namespace Tiles
{
	class ClearLayerCommand : public Command
	{
	public:
		ClearLayerCommand(const Layers& layers)
		{
			m_Index = layers.GetActiveLayer();
			m_PreviousLayer = layers.GetLayer(m_Index);
			m_NewLayer = layers.GetLayer(m_Index);
		}

		virtual void Execute(Layers& layers) override
		{
			m_NewLayer.Clear();
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
			return false;
		}

	private:
		size_t m_Index;
		Layer m_PreviousLayer;
		Layer m_NewLayer;
	};
}