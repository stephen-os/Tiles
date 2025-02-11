#pragma once

#include "TileLayer.h"

#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Base.h"

#include "json.hpp"

#include <fstream>

class Project
{
public:
    static void Save(const std::string& path, const Lumina::Ref<TileLayer>& tileLayer, const Lumina::Ref<Lumina::TextureAtlas>& atlas);
    static void Load(const std::string& path, Lumina::Ref<TileLayer>& tileLayer, Lumina::Ref<Lumina::TextureAtlas>& atlas);
};
