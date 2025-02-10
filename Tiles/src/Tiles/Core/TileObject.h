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

    void Draw();
private:
    void InitializeGeometry();
private:
    Lumina::Ref<Lumina::VertexArray> m_VertexArray;
};
