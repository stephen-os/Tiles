#include "Atlas.h"

#include <iostream>

namespace Tiles
{

    void Atlas::Create(const std::string& path)
    {
        if (!m_Texture.SetData(path))
        {
            std::cerr << "[Atlas] -> Create: Failed to load texture: " << path << "\n";
            return;
        }

        m_Path = path;
        m_IsCreated = true;

        UpdateTexCoords();
    }

    void Atlas::UpdateTexCoords()
    {
        int texturePixelWidth = m_Texture.GetWidth();
        int texturePixelHeight = m_Texture.GetHeight();

        m_TexWidth = 1.0f / static_cast<float>(m_GridWidth);
        m_TexHeight = 1.0f / static_cast<float>(m_GridHeight);

        m_TexCoords.clear();

        m_TexCoords.reserve(m_GridWidth * m_GridHeight);
        for (int y = 0; y < m_GridHeight; ++y)
        {
            for (int x = 0; x < m_GridWidth; ++x)
            {
                float uMin = x * m_TexWidth;
                float vMin = y * m_TexHeight;
                float uMax = uMin + m_TexWidth;
                float vMax = vMin + m_TexHeight;

                m_TexCoords.emplace_back(uMin, vMin, uMax, vMax);
            }
        }
    }

    void Atlas::Destroy()
    {
		m_Texture.~Texture();
        std::string m_Path = "";                
        m_GridWidth = 16;                   
        m_GridHeight = 16;                  
        m_TexWidth = 0.0f;                
        m_TexHeight = 0.0f;               
        m_IsCreated = false;               
        m_TexCoords.clear();     
    }

    void Atlas::Bind() const
    {
        m_Texture.Bind();
    }

    void Atlas::Unbind() const
    {
        m_Texture.Unbind();
    }

    glm::vec4 Atlas::GetTexCoords(int index) const
    {
        if (index < 0 || index >= static_cast<int>(m_TexCoords.size()))
        {
            std::cerr << "[Atlas] -> GetTexCoords: Invalid texture index: " << index << "\n";
            return glm::vec4(0.0f);
        }

        return m_TexCoords[index];
    }

    // Calulates the offset of the tile reative to the top left of the atlas between 0 and 1
    glm::vec2 Atlas::GetOffset(int index) const
    {
        if (index < 0 || index >= (m_GridWidth * m_GridHeight))
	    {
		    std::cerr << "[Atlas] -> GetOffset: Invalid texture index: " << index << "\n";
		    return glm::vec2(0.0f);
	    }

        int row = index / m_GridWidth;
        int col = index % m_GridWidth;

        float xOffset = col * m_TexWidth;
        float yOffset = row * m_TexHeight;

        return glm::vec2(xOffset, yOffset);
    }

    // Calculates the positon of the tile reative to the top left of the atlas
    glm::vec2 Atlas::GetPosition(int index) const
    {
        if (index < 0 || index >= m_GridWidth * m_GridHeight)
            return glm::vec2(0.0f);

        int x = index % m_GridWidth;
        int y = index / m_GridWidth;

        return glm::vec2(x, y);
    }

}