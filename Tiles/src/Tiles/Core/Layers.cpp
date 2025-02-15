#include "Layers.h"
#include <iostream>

namespace Tiles
{

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

    void Layers::RemoveLayer(size_t layer) {
        if (IsLayerInBounds(layer)) {
            m_Layers.erase(m_Layers.begin() + layer);
            if (m_ActiveLayer >= m_Layers.size() && !m_Layers.empty())
            {
                m_ActiveLayer = m_Layers.size() - 1;
            }
        }
        else {
            std::cerr << "Error: Attempted to remove an out-of-bounds layer.\n";
        }
    }

    void Layers::InsertLayer(size_t index, Layer layer) {
        if (index <= m_Layers.size())
        { // Allow inserting at end
            m_Layers.insert(m_Layers.begin() + index, std::move(layer));
        }
        else {
            std::cerr << "Error: Attempted to insert layer at an invalid index.\n";
        }
    }

    void Layers::SetTile(size_t y, size_t x, Tile tile)
    {
        if (IsLayerInBounds(m_ActiveLayer) && x < m_LayerWidth && y < m_LayerHeight)
        {
            m_Layers[m_ActiveLayer].GetTile(y, x) = tile;
        }
        else {
            std::cerr << "Error: Attempted to set tile out of bounds.\n";
        }
    }


    bool Layers::IsLayerInBounds(size_t layer)
    {
        return layer < m_Layers.size() && layer >= 0;
    }

}