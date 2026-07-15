#pragma once

#include "Commands/Command.h"

#include "Domain/LayerStack.h"
#include "Domain/TileLayer.h"

namespace Tiles
{
	// Removes a layer, snapshotting it on first Execute so Undo can reinsert it
	// at the same index with its original contents.
	class LayerDeleteCommand : public Command
	{
	public:
		LayerDeleteCommand(size_t index)
			: m_Index(index), m_HasExecuted(false)
		{
		}

		// Snapshots the layer on first run, then removes it.
		void Execute(LayerStack& layerStack) override
		{
			if (!m_HasExecuted)
			{
				m_PreviousLayer = layerStack.GetLayer(m_Index);
				m_HasExecuted = true;
			}

			layerStack.RemoveLayer(m_Index);
		}

		// Reinserts the removed layer at its original index with its contents.
		void Undo(LayerStack& layerStack) override
		{
			layerStack.InsertLayer(m_Index, m_PreviousLayer.GetName());
			layerStack.ReplaceLayer(m_Index, m_PreviousLayer);
		}

	private:
		size_t m_Index;
		TileLayer m_PreviousLayer;
		bool m_HasExecuted;
	};
}
