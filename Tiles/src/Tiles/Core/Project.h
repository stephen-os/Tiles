#pragma once

#include "Layers.h"
#include "Atlas.h"

#include "Lumina/Base.h"

#include "json.hpp"

#include <fstream>

namespace Tiles
{

    class Project
    {
    public:
        static void Save(const std::string& path, const Lumina::Ref<Layers>& layers, const Lumina::Ref<Atlas>& atlas);
        static void Load(const std::string& path, Lumina::Ref<Layers>& layers, Lumina::Ref<Atlas>& atlas);
    };

}