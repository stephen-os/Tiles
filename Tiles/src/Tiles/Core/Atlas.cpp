#include "Atlas.h"

#include <iostream>

void Atlas::Create(const std::string& path, size_t width, size_t height)
{
    if (!m_Texture.SetData(path))
    {
        std::cerr << "[Atlas] -> Create: Failed to load texture: " << path << "\n";
        return;
    }

    m_Path = path;
    m_IsCreated = true;

    UpdateTexCoords(width, height);
}

void Atlas::UpdateTexCoords(size_t width, size_t height)
{
    m_GridWidth = width;
    m_GridHeight = height;

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