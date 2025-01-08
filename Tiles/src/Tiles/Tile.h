#pragma once

#include <glm/glm.hpp>

struct Tile
{
    glm::vec4 m_Color;
    bool m_UseTexture; 
    float m_Opacity;
    int m_TextureIndex;
};