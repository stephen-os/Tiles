#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Lumina/Renderer/VertexArray.h"
#include "Lumina/Renderer/ShaderProgram.h"

#include "Lumina/Renderer/Transform.h"
#include "Lumina/Renderer/TextureAtlas.h"

class TileObject {
public:
    TileObject();
    const Lumina::Ref<Lumina::VertexArray>& GetVertexArray() { return m_VertexArray; }
private:
    void InitializeGeometry();
private:
    Lumina::Ref<Lumina::VertexArray> m_VertexArray;
};
