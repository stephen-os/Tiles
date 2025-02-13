#include "Layers.h"
#include <iostream>

void Layers::Resize(size_t width, size_t height)
{
	m_LayerWidth = width;
	m_LayerHeight = height;
}

void Layers::Clear()
{
	m_Layers.clear();
	m_ActiveLayer = 0;
}

void Layers::NewLayer()
{
	m_Layers.emplace_back(m_LayerWidth, m_LayerHeight, "Layer " + std::to_string(m_Layers.size()));
	m_ActiveLayer = m_Layers.size() - 1;
}


void Layers::DeleteLayer()
{
	if (m_Layers.size() > 0)
	{
		m_Layers.erase(m_Layers.begin() + m_ActiveLayer);
		m_ActiveLayer = (m_ActiveLayer > 0) ? m_ActiveLayer - 1 : 0;
	}
}

void Layers::ClearLayer()
{
	if (m_Layers.size() > 0)
	{
		m_Layers[m_ActiveLayer].Clear();
	}
}

bool Layers::IsLayerInBounds(size_t layer)
{
	return layer < m_Layers.size() && layer >= 0;
}