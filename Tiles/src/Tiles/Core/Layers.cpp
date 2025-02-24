#include "Layers.h"

#include "spdlog/spdlog.h"

namespace Tiles
{
	void Layers::Resize(size_t width, size_t height)
	{
		m_LayerWidth = width;
		m_LayerHeight = height;
	}

	void Layers::AddLayer()
	{
		m_Layers.emplace_back(m_LayerWidth, m_LayerHeight, "Layer " + std::to_string(m_Layers.size()));
		m_ActiveLayer = m_Layers.size() - 1;
	}

	void Layers::RemoveLayer(size_t index)
	{
		index = ResolveLayerIndex(index);
		if (IsLayerInBounds(index))
		{
			m_Layers.erase(m_Layers.begin() + index);
			if (m_ActiveLayer >= m_Layers.size() && !m_Layers.empty())
			{
				m_ActiveLayer = m_Layers.size() - 1;
			}
		}
		else
		{
			spdlog::error("Error: Attempted to remove an out-of-bounds layer.");
		}
	}

	void Layers::ClearLayer(size_t index)
	{
		index = ResolveLayerIndex(index);
		if (IsLayerInBounds(index))
			m_Layers[index].Clear();
		else
			spdlog::error("Error: Attempted to clear an out - of - bounds layer.");
	}

	void Layers::InsertLayer(Layer& layer) 
	{ 
		InsertLayer(m_ActiveLayer, layer); 
	}

	void Layers::InsertLayer(size_t index, Layer& layer)
	{
		index = ResolveLayerIndex(index);
		if (IsLayerInBounds(index))
			m_Layers.insert(m_Layers.begin() + index, std::move(layer));
		else
			spdlog::error("Error: Attempted to insert layer at an invalid index.");
	}

	bool Layers::IsLayerInBounds(size_t index) const
	{
		return index < m_Layers.size();
	}

	size_t Layers::ResolveLayerIndex(size_t index) const
	{
		return (index == std::numeric_limits<size_t>::max()) ? m_ActiveLayer : index;
	}
}
