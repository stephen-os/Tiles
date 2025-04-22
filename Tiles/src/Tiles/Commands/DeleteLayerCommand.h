#pragma once

#include "Command.h"

#include "../Core/Layers.h"
#include "../Core/Layer.h"

#include "Lumina/Core/Log.h"

namespace Tiles
{
	class DeleteLayerCommand : public Command
	{
	public:
		DeleteLayerCommand(const Layers& layer)
		{
			m_Index = layer.GetActiveLayer();
			m_PreviousLayer = layer.GetLayer(m_Index);
		}

		virtual void Execute(Layers& layers) override
		{
			LUMINA_LOG_INFO("Removing layer at index: {0}", m_Index);
			layers.RemoveLayer(m_Index);
		}

		virtual void Undo(Layers& layers) override
		{
			LUMINA_LOG_INFO("Adding layer at index: {0}", m_Index);
			layers.InsertLayer(m_Index, m_PreviousLayer);
		}

		virtual bool Validate(const Command& other) const override
		{
			return false;
		}

	private:
		size_t m_Index;
		Layer m_PreviousLayer; 
	};
}