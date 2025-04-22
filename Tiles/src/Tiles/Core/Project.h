#pragma once

#include "Lumina/Renderer/TextureAtlas.h"

#include "Layers.h"

namespace Tiles
{
    class Project
    {
    public:
        static void Save(const std::string& path, const Layers& layers, const Lumina::TextureAtlas& atlas);
        static void Load(const std::string& path, Layers& layers, Lumina::TextureAtlas& atlas);

        static const std::string& GetPath();
        static bool HasPath();
    };
}