#pragma once

#include "Lumina/Renderer/TextureAtlas.h"

#include "Layers.h"
#include "Base.h"

namespace Tiles
{

    class Project
    {
    public:
        static void Save(const std::string& path, const Shared<Layers>& layers, const Shared<Lumina::TextureAtlas>& atlas);
        static void Load(const std::string& path, Shared<Layers>& layers, Shared<Lumina::TextureAtlas>& atlas);
    };

}