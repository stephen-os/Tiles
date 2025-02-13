#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Lumina/Renderer/VertexArray.h"
#include "Lumina/Base.h"

class Quad 
{
public:
    Quad();
    ~Quad() = default;

    const Lumina::Ref<Lumina::VertexArray>& GetVertexArray() { return m_VertexArray; }
private:
    Lumina::Ref<Lumina::VertexArray> m_VertexArray;
};
