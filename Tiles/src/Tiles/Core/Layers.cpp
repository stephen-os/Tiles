#include "Layers.h"

#include "Lumina/Core/Log.h"
#include "Lumina/Core/Assert.h"

namespace Tiles
{
	void Layers::Resize(size_t width, size_t height)
	{
		m_LayerWidth = width;
		m_LayerHeight = height;
	}

	void Layers::RemoveLayer(size_t index)
	{
		index = ResolveLayerIndex(index);
		LUMINA_ASSERT(index < m_Layers.size(), "Error: Attempted to remove an out-of-bounds layer.");

		m_Layers.erase(m_Layers.begin() + index);
		if (m_ActiveLayer >= m_Layers.size() && !m_Layers.empty())
		{
			m_ActiveLayer = m_Layers.size() - 1;
		}
	}

	void Layers::ClearLayer(size_t index)
	{
		index = ResolveLayerIndex(index);
		LUMINA_ASSERT(index < m_Layers.size(), "Error: Attempted to clear an out-of-bounds layer.");

		m_Layers[index].Clear();
	}

	void Layers::InsertLayer(Layer& layer) 
	{ 
		InsertLayer(m_ActiveLayer, layer); 
	}

	void Layers::InsertLayer(size_t index, Layer& layer)
	{
		index = ResolveLayerIndex(index);
		LUMINA_ASSERT(index <= m_Layers.size(), "Error: Attempted to insert an out-of-bounds layer.");

		m_Layers.insert(m_Layers.begin() + index, std::move(layer));
	}

	void Layers::ReplaceLayer(size_t index, Layer& layer)
	{
		index = ResolveLayerIndex(index);
		LUMINA_ASSERT(index < m_Layers.size(), "Error: Attempted to replace an out-of-bounds layer.");

		m_Layers[index] = std::move(layer);
	}

	void Layers::ShiftLayerUp()
	{
		if (m_Layers.size() <= 1 || m_ActiveLayer == 0)
			return;

		std::swap(m_Layers[m_ActiveLayer], m_Layers[m_ActiveLayer - 1]);
		m_ActiveLayer--;
	}

	void Layers::ShiftLayerDown()
	{
		if (m_Layers.size() <= 1 || m_ActiveLayer >= m_Layers.size() - 1)
			return;

		std::swap(m_Layers[m_ActiveLayer], m_Layers[m_ActiveLayer + 1]);
		m_ActiveLayer++;
	}

	size_t Layers::ResolveLayerIndex(size_t index) const
	{
		return (index == std::numeric_limits<size_t>::max()) ? m_ActiveLayer : index;
	}
}
