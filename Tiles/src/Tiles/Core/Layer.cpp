#include "Layer.h"

Layer::Layer(size_t width, size_t height, const std::string& name)
{
	m_Width = width;
	m_Height = height;
	m_Name = name;

    m_Tiles.resize(m_Height, std::vector<Tile>(m_Width));
    
	Clear(); 
}

void Layer::Clear()
{
    for (auto& row : m_Tiles)
    {
        for (auto& tile : row)
        {
            tile = Tile();
        }
    }
}